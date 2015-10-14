#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>

#include "socket/socket.hpp"
#include "socket/socket_callbacks.hpp"
#include "poller/poller.hpp"
#include "common/new_connection.hpp"
#include "common/timer.hpp"

Socket::Socket(const int64_t type, const int64_t family, const int64_t fd)
{
    m_info.type   = type;
    m_info.family = family;

    m_fd = FileDescriptor::create((fd == FileDescriptor::INVALID_FD) ? ::socket(family, type, 0) : fd);
    m_info.state = (fd == FileDescriptor::INVALID_FD) ? CREATED : READY;
    m_cb.reset(new SocketCallbacks());
#ifdef DEBUG
    std::cerr << "New socket #" << *m_fd << std::endl;
#endif
}

Socket::~Socket()
{
#ifdef DEBUG
    std::cerr << "Delete socket " << *m_fd << std::endl;
#endif
}

ssize_t Socket::write(const uint8_t * data, const ssize_t data_size)
{
    return m_fd->write(data, data_size);
}

ssize_t Socket::read(uint8_t * data, const ssize_t data_size)
{
    return m_fd->read(data, data_size);
}

ssize_t Socket::read(uint8_t * data, const ssize_t data_size, bool & eof)
{
    return m_fd->read(data, data_size, eof);
}

void Socket::attach_to_poller(const std::shared_ptr<Poller> & poller)
{
    if (!poller || m_poller.lock() == poller)
        return;

    remove_from_poller();

    m_poller = poller;
}

void Socket::remove_from_poller()
{
    auto shared_poller = m_poller.lock();
    if (!shared_poller)
        return;

    shared_poller->remove_socket(get_ptr());
    m_poller.reset();
}

void Socket::update_poll_mask(const uint64_t & mask, uint64_t timeout)
{
    auto shared_poller = m_poller.lock();
    if (!shared_poller)
        throw std::runtime_error("poller is not available");
    shared_poller->update_socket(get_ptr(), mask, timeout);
}

std::shared_ptr<Poller> Socket::get_poller() const
{
    return m_poller.lock();
}

void Socket::bind(const std::string & ip, const uint16_t port) const
{
#ifdef DEBUG
    std::cerr << "Trying to bind to " << ip << ":" << port << std::endl;
#endif
    sockaddr_in saddr = get_saddr(ip.empty() ? nullptr : ip.c_str(), port);

    if (::bind(*m_fd, (sockaddr *)&saddr, sizeof(saddr)) < 0)
        throw std::runtime_error(std::string("bind failed: ").append(strerror(errno)));

    // TODO fill m_info.local_addr
}

void Socket::connect(const std::string & ip, const uint16_t port) const
{
#ifdef DEBUG
    std::cerr << "Trying to connect to " << ip << ":" << port << std::endl;
#endif
    m_info.state = CONNECTING;

    sockaddr_in saddr = get_saddr(ip.c_str(), port);

    if (::connect(*m_fd, (sockaddr *)&saddr, sizeof(saddr)) < 0)
        throw std::runtime_error(std::string("connect failed: ").append(strerror(errno)));
    m_info.state = READY;
}

void Socket::listen(const uint64_t backlog) const
{
#ifdef DEBUG
    std::cerr << "Trying to listen" << std::endl;
#endif

    if (::listen(*m_fd, backlog) < 0)
        throw std::runtime_error(std::string("listen failed: ").append(strerror(errno)));
}

NewConnection Socket::accept_() const
{
#ifdef DEBUG
    std::cerr << "Trying to accept" << std::endl;
#endif

    sockaddr_in addr;
    socklen_t addr_size = sizeof(addr);

    int64_t new_fd = 0;
    if ((new_fd = ::accept(*m_fd, (sockaddr *)&addr, &addr_size)) < 0)
    {
#ifdef DEBUG
        std::cerr << "accept failed : " << strerror(errno) << std::endl;
#endif
        return NewConnection(new_fd, addr);
    }

    return NewConnection(new_fd, addr);
}

// XXX callbacks

void Socket::read()
{
    m_cb->on_read(this);
}

void Socket::write()
{
    m_cb->on_write(this);
}

void Socket::accept()
{
    NewConnection connection;
    for (;;)
    {
        connection = accept_();
        if (connection.fd == -1)
            return;
        m_cb->on_accept(this, connection);
    }
}

void Socket::close(const uint64_t & close_reason)
{
    if (m_info.state != INVALID)
    {
#ifdef DEBUG
    std::cerr << "Closing socket #" << *m_fd << std::endl;
#endif
        m_info.state = INVALID;
        remove_from_poller();
        m_cb->on_close(this, close_reason);
    }
}

// XXX options

void Socket::set_reuseaddr(int val) const
{
    ::setsockopt(*m_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
}

void Socket::set_reuseport(int val) const
{
    ::setsockopt(*m_fd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));
}

void Socket::set_keepalive(int val) const
{
    ::setsockopt(*m_fd,SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));
}

void Socket::set_nodelay(int val) const
{
    ::setsockopt(*m_fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
}

void Socket::set_nonblock() const
{
    int flags = ::fcntl(*m_fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    ::fcntl(*m_fd, F_SETFL, flags);
}

void Socket::set_rcvbuf(int size) const
{
    ::setsockopt(*m_fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
}

void Socket::set_sndbuf(int size) const
{
    ::setsockopt(*m_fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
}

// XXX support ----------------------------------------------------------------------------------------------------
sockaddr_in Socket::get_saddr(const char * ip, const uint16_t port) const
{
    sockaddr_in saddr;
    ::memset(&saddr, 0, sizeof(saddr));

    saddr.sin_family = m_info.family;
    saddr.sin_port   = htons(port);

    if (ip == nullptr)
        saddr.sin_addr.s_addr = INADDR_ANY;
    else if (::inet_pton(saddr.sin_family, ip, &saddr.sin_addr) <= 0)
        throw std::runtime_error(std::string("inet_pton failed: ").append(strerror(errno)));

    return std::move(saddr);
}
