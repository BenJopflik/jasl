#include <iostream>
#include <string.h>
#include <assert.h>
#include <sys/eventfd.h>

#include "poller/poller.hpp"
#include "socket/socket.hpp"

#define MAX_NUMBER_OF_EPOLL_EVENTS 100

Poller::Poller()
{
    m_epoll_fd = FileDescriptor::create(::epoll_create1(0));
    m_epoll_events.resize(MAX_NUMBER_OF_EPOLL_EVENTS);

    m_event_fd = FileDescriptor::create(::eventfd(0, 0));

    epoll_event event;
    event.events = EPOLLIN | EPOLLONESHOT | EPOLLHUP | EPOLLRDHUP;
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

void Poller::update_socket(const std::shared_ptr<Socket> & socket, uint64_t event_mask)
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
        m_active_sockets.insert(std::make_pair(fd, socket));
        socket->attach_to_poller(shared_from_this());
        return;
    }

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

    epoll_ctl(*m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
    auto to_remove = m_active_sockets.find(fd);
    if (to_remove == m_active_sockets.end())
        return;

    m_removed_sockets.push_back(socket);
    signal(UPDATE);
}

uint64_t Poller::get_epoll_events()
{
    int nfds = 0;
    for (;;)
    {
        ::memset(m_epoll_events.data(), 0, m_epoll_events.size() * sizeof(epoll_event));
        nfds = ::epoll_wait(*m_epoll_fd, m_epoll_events.data(), m_epoll_events.size(), 100);// TODO rm magic numbers
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
    switch(signal)
    {
        case UPDATE:
            return signal;

        case STOP:
            m_active_sockets.clear();
            m_removed_sockets.clear();
            m_out_events.clear();
            return 0;

        default:
#ifdef DEBUG
        std::cerr << "Invalid signal " << signal << std::endl;
#endif
            return signal;
    }
}

void Poller::process_epoll_events(uint64_t number_of_epoll_events)
{
    auto iter = m_active_sockets.begin();
    FileDescriptor::FD fd = 0;

    for (uint64_t i = 0; i < number_of_epoll_events; ++i)
    {
        fd = m_epoll_events[i].data.u64;

        if (m_epoll_events[i].data.u64 == *m_event_fd)
        {
            std::cerr << "SIGNAL" << std::endl;
            eventfd_t value;
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

        PollerEvent event(iter->second);

        if (mask & EPOLLIN)
            event.action |= PollerEvent::READ;

        if (mask & EPOLLOUT)
            event.action |= PollerEvent::WRITE;

        if (mask & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
        {
            m_removed_sockets.push_back(iter->second);
            event.action |= PollerEvent::CLOSE;
        }

        m_out_events.insert(event);
    }

    if (number_of_epoll_events == m_epoll_events.size())
        m_epoll_events.resize(2 * m_epoll_events.size());
}

void Poller::process_removed_sockets()
{
    for (const auto & i : m_removed_sockets)
    {
        PollerEvent event(i);
        auto output_iter = m_out_events.find(event);
        if (output_iter != m_out_events.end())
        {
            event.action = output_iter->action | PollerEvent::CLOSE;
            m_out_events.erase(output_iter);
        }

        m_out_events.insert(event);

        auto active_socket = m_active_sockets.find(i->get_fd());
        if (active_socket != m_active_sockets.end())
            m_active_sockets.erase(active_socket);
    }

    m_removed_sockets.clear();
}

void Poller::poll()
{
    int nfds = get_epoll_events();

#ifdef DEBUG
    std::cerr << "number of epoll events: " << nfds << std::endl;
#endif

    std::lock_guard<decltype(m_lock)> lock(m_lock);

    process_epoll_events(nfds);
    process_removed_sockets();
//TODO    process_scheduler();

}

void Poller::signal(const Signal & signal) const
{
    m_event_fd->write((uint8_t *)&signal, sizeof(uint64_t));
}

PollerEvent Poller::get_next_event()
{
    if (m_out_events.empty())
        return PollerEvent();

    PollerEvent event = *m_out_events.begin();
    m_out_events.erase(m_out_events.begin());
    return event;
}
