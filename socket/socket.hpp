#pragma once

#include <memory>

#include "socket/socket_info.hpp"
#include "common/file_descriptor.hpp"

class sockaddr_in;
class SocketCallbacks;
class Poller;
class NewConnection;

class Socket
{
public:
    enum State
    {
        INVALID = 0,
        CREATED,
        CONNECTING,
        LISTENING,
        READY,

        NUM_OF_STATES
    };

public:
    virtual ~Socket();

    ssize_t write(const uint8_t * data, const ssize_t data_size);
    ssize_t read (      uint8_t * data, const ssize_t data_size);
    ssize_t read (      uint8_t * data, const ssize_t data_size, bool & eof);

    uint64_t get_current_state() const {return m_info.state;}
    int64_t  get_fd()            const {return *m_fd;}

    std::shared_ptr<Poller> get_poller() const;

    void attach_to_poller(const std::shared_ptr<Poller> & poller);
    void remove_from_poller();
    void update_poll_mask(const uint64_t & mask, uint64_t timeout = 0);

    template<class T, typename ... Args>
    void set_callbacks(Args... args)
    {
        m_cb.reset(new T(std::forward<Args>(args)...));
    }

// options
    void set_reuseaddr(int val = 1) const;
    void set_reuseport(int val = 1) const;
    void set_keepalive(int val = 1) const;
    void set_nodelay  (int val = 1) const;
    void set_nonblock ()            const;

    void set_rcvbuf(int) const;
    void set_sndbuf(int) const;

// callbacks (XXX const?)
    virtual void read  ();
    virtual void write ();
    virtual void accept();
    virtual void close (const uint64_t & close_reason = 0);
// ----------------------

protected:
    Socket(const int64_t type, const int64_t family, const int64_t fd = FileDescriptor::INVALID_FD);

    virtual std::shared_ptr<Socket> get_ptr() = 0;

    void bind   (const std::string & ip, const uint16_t port) const;
    void connect(const std::string & ip, const uint16_t port) const;
    void listen (const uint64_t backlog = 1000) const;
    NewConnection accept_() const;

private:
    sockaddr_in get_saddr(const char * ip = nullptr, const uint16_t port = 0) const;

private:
    std::weak_ptr<Poller>            m_poller;
    std::shared_ptr<FileDescriptor>  m_fd;
    std::unique_ptr<SocketCallbacks> m_cb;
    uint64_t m_close;
    mutable SocketInfo m_info; // XXX change to shared_ptr?

}; // class Socket
