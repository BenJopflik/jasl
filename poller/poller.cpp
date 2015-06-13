#include <unistd.h>
#include <assert.h>
#include <sys/eventfd.h>
#include <mutex>

#include "socket/socket.hpp"
#include "poller/poller.hpp"


Poller::Poller(const Params & params) : m_params(params), m_events(1000), m_updates(1000)
{
    m_epoll_fd = epoll_create1(0);
    m_epoll_events.reset(new epoll_event[m_params.queue_size]);
    m_epoll_events_buffer_size = sizeof(epoll_event) * m_params.queue_size;

    m_stop_fd = eventfd(0, 0);

    epoll_event event;
    event.events = EPOLLIN | EPOLLONESHOT | EPOLLHUP | EPOLLRDHUP;
    event.data.u64 = m_stop_fd;

    epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_stop_fd, &event);
    m_active_sockets.insert(std::make_pair(m_stop_fd, Action(event.events, 0, 0)));
}

Poller::~Poller()
{
    ::close(m_epoll_fd);
}

void Poller::erase(uint64_t fd)
{
    epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
    update(fd, Action(Action::DELETE_MASK, 0, 0));
}

bool Poller::update(int64_t fd, const Action & action)
{
    return m_updates.push(std::make_pair(fd, action));
}

void Poller::update_()
{
    std::pair<int64_t, Action> update;

    while (m_updates.pop(update))
    {
        auto active_socket = m_active_sockets.find(update.first);

        if (update.second.mask == Action::DELETE_MASK)
        {
            m_active_sockets.erase(update.first);
            continue;
        }

        epoll_event event;
        event.events = update.second.mask | EPOLLHUP | EPOLLRDHUP;
        event.data.u64 = update.first;

        if (active_socket == m_active_sockets.end())
        {
            epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, update.first, &event);
            m_active_sockets.insert(std::make_pair(update.first, update.second));
        }
        else
        {
            epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, update.first, &event);
            active_socket->second = update.second;
        }
    }
}

//void Poller::add_delayed(const TimerEvent & event)
//{
//    m_delyed_actions.insert(event);
//}

void Poller::stop()
{
    eventfd_write(m_stop_fd, uint64_t(0x0abcdef0));
}

bool Poller::poll()
{
//    auto now = m_timer.elapsed_milliseconds(true);
//    while (!m_delyed_actions.empty() && m_delyed_actions.begin().check(now))
//        m_delyed_actions.erase(m_delyed_actions.begin());

    // POLL
    int nfds = 0;
    for (;;)
    {
        memset(m_epoll_events.get(), 0, m_epoll_events_buffer_size);
        nfds = epoll_wait(m_epoll_fd, m_epoll_events.get(), m_params.queue_size, m_params.poll_interval);
        if (nfds < 0)
        {
            if (errno == EINTR)
                continue;

            std::cerr << "epoll_wait failed : " << strerror(errno) << std::endl;
            exit(1);
        }

        break;
    }

    auto iter = m_active_sockets.begin();

    for (int i = 0; i < nfds; ++i)
    {
        std::cerr << "event on " << m_epoll_events[i].data.u64 << std::endl;
        // process action
        iter = m_active_sockets.find(m_epoll_events[i].data.u64);
        if (iter == m_active_sockets.end())
        {
            std::cerr << m_epoll_events[i].data.u64 << " not found" << std::endl;
            assert(false);
        }

        if (iter->first == m_stop_fd)
        {
            Event event;
            event.mask = Event::STOP;
            m_events.push(event);

            return false;
        }

        auto mask = m_epoll_events[i].events;

        Event event;
        event.fd = m_epoll_events[i].data.u64;
        event.user_data = iter->second.user_data;

        if (mask & EPOLLIN)
        {
            event.mask |= Event::READ | ((iter->second.mask & EPOLLONESHOT) ? Event::REARM : 0);
        }

        if (mask & EPOLLOUT)
        {
            event.mask |= Event::WRITE | ((iter->second.mask & EPOLLONESHOT) ? Event::REARM : 0);
        }

        if (mask & EPOLLERR)
        {
            event.mask = Event::ERROR;
            mask |= EPOLLHUP;
        }

        if (mask & (EPOLLHUP | EPOLLRDHUP))
        {
            m_active_sockets.erase(iter);
            event.mask |= Event::CLOSE;
            event.mask &= ~Event::REARM;
        }

        if (event.mask && !m_events.push(event))
        {
            std::cerr << "Event queue is full!" << std::endl;
//            update(iter->first, iter->second);
        }

    }

    update_();

    return true;
}

bool Poller::get_event(Event & event)
{
    return m_events.pop(event);
}

std::unique_ptr<Worker> Poller::run_poll_in_thread()
{
    auto loop_function = [=](){while (poll()){}};
    auto worker = Worker::create(2);
    worker->add(loop_function);
    worker->stop();
    return worker;
}

