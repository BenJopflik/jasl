#pragma once

#include <iostream>
#include <errno.h>
#include <vector>

#include <chrono>
#include <memory>
#include <cassert>
#include <atomic>

#include "socket_base.hpp"
#include "socket_callbacks.hpp"

class Poller;

class Socket : public SocketBase
{

public:
    virtual void read();
    virtual void write();
    virtual void error();
    virtual void rearm();
    virtual void close() override;

    virtual uint64_t read(uint8_t * data, uint64_t data_size, bool & eof);
    virtual uint64_t write(const uint8_t * data, uint64_t data_size);

//    virtual uint64_t receive_from(uint8_t * data, uint64_t data_size, sockaddr_in * from = nullptr);
//    virtual uint64_t send_to(uint8_t * data, uint64_t data_size, sockaddr_in * to = nullptr):

    void add_to_poller(uint64_t mask, Poller * = nullptr);
    void remove_from_poller();

    std::string get_last_error() const;
    std::string get_remote_addr() const;
    Poller * get_poller() const;

    template<class T, typename ... Args>
    void set_callbacks(Args... args)
    {
        m_cb.reset(new T(args...));
    }


protected:
    Socket();
    virtual ~Socket();
    virtual void operation_timeout() const;

// options
    void set_reuseaddr(int val = 1) const;
    void set_keepalive(int val = 1) const;
    void set_nodelay(int val = 1) const;
    void set_nonblock() const;

protected:
    Poller * m_poller {nullptr};
    std::unique_ptr<SocketCallbacks> m_cb;

};
