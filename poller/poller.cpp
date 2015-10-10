#include <unistd.h>
#include <assert.h>
#include <sys/eventfd.h>
#include <mutex>

#include "socket/socket.hpp"
#include "poller/poller.hpp"

Poller::Poller()
{
    m_epoll_fd = epoll_create1(0);
    m_epoll_events.resize(MAX_NUMBER_OF_EVENTS);
    m_signal_fd = eventfd(0, 0);

    epoll_event event;
    event.events = EPOLLIN | EPOLLONESHOT | EPOLLHUP | EPOLLRDHUP;
    event.data.u64 = m_signal_fd;

    epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_signal_fd, &event);
}

Poller::~Poller()
{
    ::close(m_signal_fd);
    ::close(m_epoll_fd);
}

void Poller::add_to_scheduler(Socket * socket, uint64_t action, uint64_t timeout)
{
    auto iter = m_sockets.find(socket->get_fd());

    if (action & EPOLLIN)
        iter->second.read_timeout = m_timer.milliseconds_from_epoch() + timeout;

    if (action & EPOLLOUT)
        iter->second.write_timeout = m_timer.milliseconds_from_epoch() + timeout;

    m_scheduler.add_event(iter->second.get_nearest_timeout(), EventPayload(iter->second.socket->get_fd()));
}

void Poller::update_socket(Socket * socket, uint64_t action, bool add, uint64_t timeout)
{
    update_socket(socket, action, add);
    std::lock_guard<decltype(m_lock)> lock(m_lock);
    add_to_scheduler(socket, action, timeout);
}

void Poller::update_socket(Socket * socket, uint64_t action, bool add)
{
    epoll_event event;
    event.events = action | EPOLLHUP | EPOLLRDHUP;
    event.data.u64 = socket->get_fd();

#ifdef DEBUG
    {
        std::lock_guard<decltype(m_lock)> lock(m_lock);
        auto iter = m_sockets.find(socket->get_fd());
        const auto END = m_sockets.end();
        if ((add && iter != END) || (!add && iter == END))
            assert(false && "invalid operation");
    }
#endif

    if (add)
    {
        std::lock_guard<decltype(m_lock)> lock(m_lock);
        epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, event.data.u64, &event);
        m_sockets.insert(std::make_pair(socket->get_fd(), SocketContext(std::shared_ptr<Socket>(socket))));
        socket->attached_to_poller(this);
        add_to_scheduler(socket, action, DEFAULT_TIMEOUT_MS);
        return;
    }

    std::lock_guard<decltype(m_lock)> lock(m_lock);
    epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, socket->get_fd(), &event);
    add_to_scheduler(socket, action, DEFAULT_TIMEOUT_MS);
}


void Poller::remove_socket(Socket * socket)
{
    std::lock_guard<decltype(m_lock)> lock(m_lock);
    epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, socket->get_fd(), nullptr);
    m_scheduler.delete_event(EventPayload(socket->get_fd()));
    auto to_remove = m_sockets.find(socket->get_fd());
    if (to_remove == m_sockets.end())
        return;

    m_removes.push_back(to_remove->second.socket);
    signal(UPDATE);
}

void Poller::poll()
{
    // POLL
    int nfds = 0;
    uint64_t epoll_wait_time = 0;
    for (;;)
    {
        memset(m_epoll_events.data(), 0, m_epoll_events.size() * sizeof(epoll_event));
        epoll_wait_time = m_scheduler.ms_to_next_event();
        if (epoll_wait_time == 0 || !m_removes.empty())
            break;

        nfds = epoll_wait(m_epoll_fd, m_epoll_events.data(), m_epoll_events.size(), epoll_wait_time);
        if (nfds < 0)
        {
            if (errno == EINTR)
                continue;

            std::cerr << "epoll_wait failed : " << strerror(errno) << std::endl;
            exit(1);
        }

        break;
    }

    std::cerr << nfds << std::endl;

    {
        std::lock_guard<decltype(m_lock)> lock(m_lock);

        auto iter = m_sockets.begin();
        for (int i = 0; i < nfds; ++i)
        {
            std::cerr << "event on " << m_epoll_events[i].data.u64 << std::endl;

            if (m_epoll_events[i].data.u64 == m_signal_fd)
            {
                std::cerr << "SIGNAL" << std::endl;
                eventfd_t value;
                eventfd_read(m_signal_fd, &value);
//XXX                PROCESS_SIGNAL
//                    continue / break;
                continue;
            }

            iter = m_sockets.find(m_epoll_events[i].data.u64);
            if (iter == m_sockets.end())
            {
                std::cerr << m_epoll_events[i].data.u64 << " not found" << std::endl;
                assert(false);
            }

            auto mask = m_epoll_events[i].events;

            PollerEvent event(iter->second.socket);

            if (mask & EPOLLIN)
            {
                iter->second.update_timeout(true);
                event.action |= PollerEvent::READ;
            }

            if (mask & EPOLLOUT)
            {
                iter->second.update_timeout(false);
                event.action |= PollerEvent::WRITE;
            }

            if (mask & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
            {
                m_scheduler.delete_event(iter->first);
                m_sockets.erase(iter);
                event.action = PollerEvent::CLOSE;
            }

            m_scheduler.add_event(iter->second.get_nearest_timeout(), iter->first);
            m_out_events.insert(event);
        }

        for (const auto & i : m_removes)
        {
            PollerEvent event(i);
            auto output_iter = m_out_events.find(event);
            if (output_iter != m_out_events.end())
                m_out_events.erase(output_iter);

            m_out_events.insert(PollerEvent(i, PollerEvent::CLOSE));

            m_scheduler.delete_event(i->get_fd());

            auto active_iter = m_sockets.find(i->get_fd());
            if (active_iter != m_sockets.end())
                m_sockets.erase(active_iter);
        }

        m_removes.clear();

        if (nfds == m_epoll_events.size())
            m_epoll_events.resize(2 * m_epoll_events.size());

        uint64_t current_time = m_timer.milliseconds_from_epoch();
        EventPayload payload;
        while (m_scheduler.get_next_event(payload, current_time))
        {
            iter = m_sockets.find(payload.i64);
            if (iter == m_sockets.end())
                continue;

            auto timeout_type = iter->second.get_timeout_type(current_time);
            if (!timeout_type)
                continue;

            auto out_event = m_out_events.find(PollerEvent(iter->second.socket));
            if (out_event == m_out_events.end())
            {
                m_out_events.insert(PollerEvent(iter->second.socket, timeout_type));
                continue;
            }

            uint64_t out_event_type = out_event->action;
            if (out_event_type & PollerEvent::CLOSE)
                continue;

            if (out_event_type & PollerEvent::READ && timeout_type & PollerEvent::TIMEOUT_READ)
                timeout_type &= ~PollerEvent::TIMEOUT_READ;

            if (out_event_type & PollerEvent::WRITE && timeout_type & PollerEvent::TIMEOUT_WRITE)
                timeout_type &= ~PollerEvent::TIMEOUT_WRITE;

            if (timeout_type)
            {
                PollerEvent modified_event = *out_event;
                m_out_events.erase(out_event);
                modified_event.action |= timeout_type;
                m_out_events.insert(modified_event);
            }
        }
    }
}


PollerEvent Poller::get_next_event()
{
    if (m_out_events.empty())
        return PollerEvent();
    PollerEvent event = *m_out_events.begin();
    m_out_events.erase(m_out_events.begin());
    return event;
}

