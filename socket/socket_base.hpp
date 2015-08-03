#pragma once
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "common/timer.hpp"
#include "common/ip_addr.hpp"
#include "common/new_connection.hpp"
#include "common/non_copyable.hpp"

class SocketBase : public NonCopyable
{
public:
    virtual void close();

protected:
    using Fd = int64_t;

    enum
    {
        INVALID_FD = -1,
        INVALID_TYPE = -1,
    };

protected:
    SocketBase();
    virtual ~SocketBase();

    void create();
    void listen() const;
    void connect(const std::string & ip_addr, uint16_t port);
    void connect(const sockaddr_in & saddr);
    void bind(const std::string & ip_addr, uint16_t port);
    NewConnection accept();

protected:
    Fd m_fd {INVALID_FD};

    int64_t m_family_type {INVALID_TYPE};
    int64_t m_socket_type {INVALID_TYPE};

    IpAddr m_remote_ip;
    IpAddr m_local_ip;
};
