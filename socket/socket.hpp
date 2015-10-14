#pragma once

#include <memory>

#include "socket_info.hpp"
#include "common/file_descriptor.hpp"

class sockaddr_in;
class SocketCallbacks;
class Poller;

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
    ssize_t read (      uint8_t * data, const ssize_t data_size, bool & eof);

    uint64_t get_current_state() const {return m_info.state;}
    int64_t  get_fd()            const {return *m_fd;}

    void attach_to_poller(const std::shared_ptr<Poller> & poller);
    void remove_from_poller();

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

// callbacks
    virtual void read ();
    virtual void write();
    virtual void error();
    virtual void close();
// ----------------------

protected:
    Socket(const int64_t type, const int64_t family);

    void bind   (const std::string & ip, const uint16_t port);
    void connect(const std::string & ip, const uint16_t port);
    void listen (const uint64_t backlog = 1000) const;
// TODO accept

    virtual std::shared_ptr<Socket> get_ptr() = 0;

private:
    sockaddr_in get_saddr(const char * ip = nullptr, const uint16_t port = 0);

private:
    std::shared_ptr<Poller>          m_poller;
    std::shared_ptr<FileDescriptor>  m_fd;
    std::unique_ptr<SocketCallbacks> m_cb;

    SocketInfo m_info; // XXX change to shared_ptr?

};
