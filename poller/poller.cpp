#include <iostream>
#include <string.h>
#include <assert.h>
#include <sys/eventfd.h>

#include "poller/poller.hpp"
#include "socket/socket.hpp"

#define MAX_NUMBER_OF_EPOLL_EVENTS 100

std::shared_ptr<Poller> Poller::create()
{
    return std::shared_ptr<Poller>(new Poller());
}

Poller::Poller()
{
    m_epoll_fd = FileDescriptor::create(::epoll_create1(0));
    m_epoll_events.resize(MAX_NUMBER_OF_EPOLL_EVENTS);

    m_event_fd = FileDescriptor::create(::eventfd(0, EFD_NONBLOCK));

    epoll_event event;
    event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
    event.data.u64 = *m_event_fd;

    ::epoll_ctl(*m_epoll_fd, EPOLL_CTL_ADD, *m_event_fd, &event);

#ifdef DEBUG
    std::cerr << "New Poller # " << *m_epoll_fd << std::endl;
#endif
}

Poller::~Poller()
{
#ifdef DEBUG
    std::cerr << "Delete Poller # " << *m_epoll_fd << std::endl;
#endif
}

void Poller::add_to_scheduler(SocketContext  & context,
                              const uint64_t & action,
                              const uint64_t & timeout_step)
{
    if (action & EPOLLIN)
    {
        context.read_timeout.update_timeout_step(timeout_step);
        if (context.read_timeout.time == SocketContext::INVALID_TIME)
            context.read_timeout.time = m_timer.milliseconds_from_epoch() + timeout_step;
    }

    if (action & EPOLLOUT)
    {
        context.write_timeout.update_timeout_step(timeout_step);
        if (context.write_timeout.time == SocketContext::INVALID_TIME)
            context.write_timeout.time = m_timer.milliseconds_from_epoch() + timeout_step;
    }

    m_scheduler.add_event(context.get_nearest_timeout(), EventPayload(context.socket->get_fd()));
}

void Poller::update_socket(const std::shared_ptr<Socket> & socket,
                           const uint64_t & event_mask,
                           const uint64_t & timeout_step)
{
    if (!socket)
    {
#ifdef DEBUG
        std::cerr << "invalid socket ptr in update" << std::endl;
#endif
        return;
    }

    auto fd = socket->get_fd();

    epoll_event event;
    event.events = event_mask | EPOLLHUP | EPOLLRDHUP;
    event.data.u64 = fd;

    std::lock_guard<decltype(m_lock)> lock(m_lock);

    auto iter = m_active_sockets.find(fd);
    if (iter == m_active_sockets.end())
    {
        ::epoll_ctl(*m_epoll_fd, EPOLL_CTL_ADD, fd, &event);
        auto iter = m_active_sockets.insert(std::make_pair(fd, SocketContext(socket)));
        if (timeout_step != SocketContext::INVALID_TIME)
            add_to_scheduler(iter.first->second, event_mask, timeout_step ? timeout_step : SocketContext::DEFAULT_TIMEOUT_MS);
        socket->attach_to_poller(shared_from_this());
        signal(UPDATE);
        return;
    }

    if (timeout_step != SocketContext::INVALID_TIME)
        add_to_scheduler(iter->second, event_mask, timeout_step ? timeout_step : SocketContext::DEFAULT_TIMEOUT_MS);

    ::epoll_ctl(*m_epoll_fd, EPOLL_CTL_MOD, fd, &event);
}

void Poller::remove_socket(const std::shared_ptr<Socket> & socket)
{
    if (!socket)
    {
#ifdef DEBUG
        std::cerr << "invalid socket ptr in remove" << std::endl;
#endif
        return;
    }

    auto fd = socket->get_fd();

    std::lock_guard<decltype(m_lock)> lock(m_lock);

    ::epoll_ctl(*m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
    auto to_remove = m_active_sockets.find(fd);
    if (to_remove == m_active_sockets.end())
        return;

    m_scheduler.delete_event(EventPayload(fd));

    PollerEvent event(socket, PollerEvent::CLOSE);
    auto output_iter = m_out_events.find(event);
    if (output_iter != m_out_events.end())
        m_out_events.erase(output_iter);

    m_active_sockets.erase(to_remove);
}

uint64_t Poller::get_epoll_events()
{
    int nfds = 0;
    uint64_t epoll_wait_time = 0;

    for (;;)
    {
        epoll_wait_time = m_scheduler.ms_to_next_event();
        if (!epoll_wait_time)
            break;

        ::memset(m_epoll_events.data(), 0, m_epoll_events.size() * sizeof(epoll_event));
#ifdef DEBUG
        std::cerr << "epoll wait time " << epoll_wait_time << std::endl;
#endif
        nfds = ::epoll_wait(*m_epoll_fd, m_epoll_events.data(), m_epoll_events.size(), epoll_wait_time);

        if (nfds < 0)
        {
            if (errno == EINTR)
                continue;

            std::cerr << "epoll_wait failed : " << strerror(errno) << std::endl;
            ::exit(1);
        }

        break;
    }

    return nfds;
}

uint64_t Poller::process_signal(const uint64_t & signal)
{
    if (signal >= Signal::STOP)
    {
        std::cerr << "STOP" << std::endl;
        m_active_sockets.clear();
        m_out_events.clear();
    }
    else
    {
#ifdef DEBUG
        std::cerr << "Update signal " << signal << std::endl;
#endif
        return signal;
    }

    return 0;
}

void Poller::process_epoll_events(uint64_t number_of_epoll_events)
{
    auto iter = m_active_sockets.begin();
    FileDescriptor::FD fd = 0;
    auto current_time = m_timer.milliseconds_from_epoch();

    for (uint64_t i = 0; i < number_of_epoll_events; ++i)
    {
        fd = m_epoll_events[i].data.u64;

        if (m_epoll_events[i].data.u64 == *m_event_fd)
        {
            eventfd_t value = 0;
            m_event_fd->read((uint8_t*)&value, sizeof(eventfd_t));

            if (process_signal(value))
                continue;
            return;
        }

#ifdef DEBUG
        std::cerr << "event on " << fd << std::endl;
#endif

        iter = m_active_sockets.find(fd);
        if (iter == m_active_sockets.end())
        {
            std::cerr << "invalid socket descriptor " << fd << std::endl;
            assert(false);
        }

        auto mask = m_epoll_events[i].events;

        PollerEvent event(iter->second.socket);

        if (mask & EPOLLIN)
        {
            iter->second.read_timeout.update_timeout(current_time);
            event.action |= PollerEvent::READ;
        }

        if (mask & EPOLLOUT)
        {
            iter->second.write_timeout.update_timeout(current_time);
            event.action |= PollerEvent::WRITE;
        }

        if (mask & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
        {
            event.action |= PollerEvent::CLOSE;
            m_scheduler.delete_event(EventPayload(iter->first));

            event.close_reason |= (mask & EPOLLHUP)   ? PollerEvent::PEER_SHUTDOWN       : 0;
            event.close_reason |= (mask & EPOLLRDHUP) ? PollerEvent::UNEXPECTED_SHUTDOWN : 0;
            event.close_reason |= (mask & EPOLLERR)   ? PollerEvent::SOCKET_ERROR        : 0;
        }

        assert(event.action);

        m_scheduler.add_event(iter->second.get_nearest_timeout(), iter->first);
        m_out_events.insert(event);
    }

    if (number_of_epoll_events == m_epoll_events.size())
        m_epoll_events.resize(2 * m_epoll_events.size());
}

void Poller::process_scheduler()
{
    uint64_t current_time = m_timer.milliseconds_from_epoch();
    auto iter = m_active_sockets.begin();
    EventPayload payload;

    while (m_scheduler.get_next_event(payload, current_time))
    {
        iter = m_active_sockets.find(payload.i64);
        if (iter == m_active_sockets.end())
            continue;

        auto timeout_type = iter->second.get_timeout_type(current_time);
        if (!timeout_type)
            continue;

        auto out_event = m_out_events.find(PollerEvent(iter->second.socket));
        if (out_event == m_out_events.end())
        {
            m_out_events.insert(PollerEvent(iter->second.socket, PollerEvent::CLOSE, timeout_type));
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
            m_scheduler.delete_event(EventPayload(iter->second.socket->get_fd()));

            PollerEvent modified_event = *out_event;
            m_out_events.erase(out_event);
            modified_event.action |=  PollerEvent::CLOSE;
            modified_event.close_reason = timeout_type;
            m_out_events.insert(modified_event);
        }
    }
}

void Poller::poll()
{
    int nfds = get_epoll_events();

#ifdef DEBUG
    std::cerr << "number of epoll events: " << nfds << std::endl;
#endif

    std::lock_guard<decltype(m_lock)> lock(m_lock);
    process_epoll_events(nfds);
    process_scheduler();
}

void Poller::signal(const Signal & signal) const
{
#ifdef DEBUG
    std::cerr << "send signal" << std::endl;
#endif
    uint64_t sig(signal);
    m_event_fd->write((uint8_t *)&sig, sizeof(uint64_t));
}

PollerEvent Poller::get_next_event()
{
    if (m_out_events.empty())
        return PollerEvent();

    PollerEvent event = *m_out_events.begin();
    m_out_events.erase(m_out_events.begin());
    return event;
}
