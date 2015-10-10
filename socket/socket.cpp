#include "socket.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <stdexcept>
#include <netinet/tcp.h>
#include "poller/poller.hpp"
#include "common/to_string.hpp"
#include "common/new_connection.hpp"

//http://kovyrin.net/2006/04/13/epoll-asynchronous-network-programming/
//http://stackoverflow.com/questions/3192940/best-socket-options-for-client-and-sever-that-continuously-transfer-data

Socket::Socket()
{
    m_cb.reset(new SocketCallbacks());
}

Socket::~Socket()
{
    ::close(m_fd);
    std::cerr << "~Socket " << m_fd << std::endl;
}

void Socket::add_to_poller(uint64_t mask, Poller * poller)
{
    bool new_poller = poller && poller != m_poller;
    if (poller)
    {
        if (poller != m_poller)
        {
            if (m_poller)
                m_poller->remove_socket(this);
        }

        m_poller = poller;
    }

    if (m_poller)
        m_poller->update_socket(this, mask, new_poller);
}

void Socket::remove_from_poller()
{
    m_poller->remove_socket(this);
    m_poller = nullptr;
}

uint64_t Socket::read(uint8_t * data, uint64_t data_size, bool & eof)
{
    assert(data && data_size && "invalid input args");

    int64_t  nbytes = 0;
    uint64_t offset = 0;

    eof = false;

    while ((nbytes = ::read(m_fd, data, data_size - offset)))
    {
        if (nbytes > 0)
        {
            offset += nbytes;
            if (offset >= data_size)
                break;
        }
        else if (nbytes == 0)
        {
            // eof
            eof = true;
            break;
        }
        else
        {
            if (errno == EINTR)
                continue;

            if (errno != EWOULDBLOCK && errno != EAGAIN)
            {
                std::cerr << "read failed : " << strerror(errno) << std::endl;
                throw std::runtime_error(strerror(errno));
            }

            break;
        }
    }

    return offset;
}


uint64_t Socket::write(const uint8_t * data, uint64_t data_size)
{
    assert(data && data_size && "invalid input args");

    uint64_t offset = 0;
    int nbytes = 0;

    while ((nbytes = ::write(m_fd, data + offset, data_size - offset)))
    {
        if (nbytes < 0)
        {
            if (errno != EWOULDBLOCK && errno != EAGAIN)
            {
                std::cerr << "write failed : " << strerror(errno) << std::endl;
                throw std::runtime_error(strerror(errno));
            }
            else
            {
                break;
            }
        }
        else
        {
            offset += nbytes;
            if (offset >= data_size)
                break;
        }
    }
    return offset;
}

// TODO add rcvbuf sndbuf linger options

void Socket::set_reuseaddr(int val) const
{
    ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
}

void Socket::set_reuseport(int val) const
{
    ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));
}

void Socket::set_keepalive(int val) const
{
    ::setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));
}

void Socket::set_nodelay(int val) const
{
    ::setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
}

void Socket::set_nonblock() const
{
    // nonblock
    int flags = ::fcntl(m_fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    ::fcntl(m_fd, F_SETFL, flags);

    // close-on-exec
    flags = ::fcntl(m_fd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ::fcntl(m_fd, F_SETFD, flags);
}

void Socket::operation_timeout() const
{
    // TODO check m_status and return error
//    const uint64_t m_step {0};
}

void Socket::set_rcv_buffer(int size) const
{
    ::setsockopt(m_fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
}

void Socket::set_snd_buffer(int size) const
{
    ::setsockopt(m_fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
}

std::string Socket::get_last_error() const
{
    std::string output;
    int val = 0;
    socklen_t len = sizeof(val);
    ::getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &val, &len);
    output = strerror(val);

    return std::move(output);
}

std::string Socket::get_remote_addr() const
{
    return std::string(m_remote_ip.full_addr);
}

Poller * Socket::get_poller() const
{
    return m_poller;
}


// callbacks
void Socket::read()
{
    if (m_state == CONNECTING)
    {
        m_state = ACTIVE;
//        m_cb->on_connected(this);
    }

    if (m_state == ACTIVE)
        m_cb->on_read(this);
}

void Socket::write()
{
    if (m_state == CONNECTING)
    {
        m_state = ACTIVE;
//        m_cb->on_connected(this);
    }

    if (m_state == ACTIVE)
        m_cb->on_write(this);
}

void Socket::accept()
{
    NewConnection connection;
    for (;;)
    {
        connection = SocketBase::accept();
        if (connection.fd == -1)
            return;
        m_cb->on_accept(this, connection);
    }
}

void Socket::accept()
{
    m_cb->on_accept(this, SocketBase::accept());
}

void Socket::close(bool clear_memory)
{
    if (m_state != CLOSED)
    {
        //#ifdef DEBUG
        std::cerr << "Closing " << socket_type_to_string(m_socket_type) << " socket #" << m_fd << std::endl;
        //#endif

        int64_t fd = m_fd;

        if (m_poller)
            remove_from_poller();

//        m_fd = INVALID_FD;
        m_cb->on_close(this, fd);
        //    if (clear_memory)
        //        destroy();

        m_state = CLOSED;
    }
}

void Socket::error()
{
    m_cb->on_error(this);
}

void Socket::rearm()
{
    m_cb->on_rearm(this);
}
// callbacks

void Socket::destroy()
{
    delete this;
}


