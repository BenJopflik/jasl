#include "socket_base.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdexcept>

#include "common/to_string.hpp"


SocketBase::SocketBase()
{

}

SocketBase::~SocketBase()
{
    if (m_fd != INVALID_FD)
        close();
}

void SocketBase::close()
{

}

void SocketBase::create()
{
    m_fd = ::socket(m_family_type, m_socket_type, 0);
    if (m_fd < INVALID_FD)
    {
        std::cerr << "Unable to create " << socket_type_to_string(m_socket_type) << " socket: " << strerror(errno) << std::endl;
        exit(1);
    }

#ifdef DEBUG
    std::cerr << "New " << socket_type_to_string(m_socket_type) << " socket #" << m_fd << std::endl;
#endif
}

static void fill_sin_addr(const std::string & source, sockaddr_in & addr)
{
    // TODO if not an ip, use getaddrinfo to get ip
    if (::inet_pton(addr.sin_family, source.c_str(), &addr.sin_addr) <= 0)
    {
        std::cerr << "inet_pton failed : " << strerror(errno) << std::endl;
        exit(1);
    }
}

// TODO create poller class
static bool personal_poll(int64_t fd, uint64_t type, uint64_t timeout_in_milliseconds)
{
    bool ret = false;

    auto epoll_fd = epoll_create(1);

    epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = type | EPOLLHUP | EPOLLRDHUP;
    event.data.fd = fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);

    int nfds = 0;
    epoll_event events[1];
    memset(events, 0, sizeof(events));
    Timer timer;
    uint64_t timeout = timeout_in_milliseconds;
    uint64_t elapsed = 0;
    for (;;)
    {
        if (timeout < elapsed)
            timeout = 0;

        nfds = epoll_wait(epoll_fd, events, 1, timeout);
        if (nfds < 0)
        {
            if (errno == EINTR)
            {
                elapsed = timer.elapsed_milliseconds();
                if ( elapsed < timeout_in_milliseconds)
                    continue;
            }

            std::cerr << "epoll failed : " << strerror(errno) << std::endl;
            exit(1);
        }

        ret = bool(nfds);

        break;
    }

    close(epoll_fd);
    return ret;
}

void SocketBase::connect(const std::string & ip_addr, uint16_t port)//, uint64_t timeout)
{
#ifdef DEBUG
    std::cerr << "Trying to connect to " << ip_addr << ":" << port << std::endl;
#endif

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = m_family_type;
    addr.sin_port = htons(port);

    fill_sin_addr(ip_addr, addr);
    connect(addr);

    m_remote_ip.set(addr);
}

void SocketBase::connect(const sockaddr_in & saddr)
{
    if (::connect(m_fd, (sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        if (errno != EINPROGRESS || !personal_poll(m_fd, EPOLLOUT, 1000))
            throw std::runtime_error(std::string("connection failed: ").append(strerror(errno)));
    }

}

// TODO add support for AF_LOOPBACK etc.

void SocketBase::bind(const std::string & ip, uint16_t port)
{
#ifdef DEBUG
    std::cerr << "Trying to bind to " << ip << ":" << port << std::endl;
#endif

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = m_family_type;
    addr.sin_port = htons(port);

    if (!ip.empty())
        fill_sin_addr(ip, addr);
    else
        addr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(m_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "bind failed : " << strerror(errno) << std::endl;
        exit(1);
    }

    m_local_ip.set(ip, port);
}

void SocketBase::listen() const
{
#ifdef DEBUG
    std::cerr << "Trying to listen" << std::endl;
#endif

    if (::listen(m_fd, 0) < 0)
    {
        std::cerr << "listen failed : " << strerror(errno) << std::endl;
        exit(1);
    }
}

NewConnection SocketBase::accept()
{
#ifdef DEBUG
    std::cerr << "Trying to accept" << std::endl;
#endif

    sockaddr_in addr;
    socklen_t addr_size = sizeof(addr);

    int64_t new_fd = 0;
    if ((new_fd = ::accept(m_fd, (sockaddr *)&addr, &addr_size)) < 0)
    {
        std::cerr << "accept failed : " << strerror(errno) << std::endl;
        exit(1);
    }

    return NewConnection(new_fd, addr);
}

