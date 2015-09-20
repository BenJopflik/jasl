#pragma once
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <set>
#include <functional>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <list>

#include "memory/spsc_queue.hpp"
#include "memory/mpsc_queue.hpp"
#include "socket/socket_base.hpp"
#include "thread/worker.hpp"
//#include "common/event.hpp"
#include "thread/spinlock.hpp"
#include "poller/action.hpp"
#include "poller/scheduler.hpp"
#include "poller/poller_event.hpp"

#define DEFAULT_TIMEOUT_MS 30000
#define INVALID_TIME uint64_t(-1)

class Socket;
struct SocketContext
{
public:
    SocketContext(std::shared_ptr<Socket> socket) : socket(socket) {}
    uint64_t get_nearest_timeout() const
    {
        assert(read_timeout != INVALID_TIME || write_timeout != INVALID_TIME);
        return std::min(read_timeout, write_timeout);
    }

    void update_timeout(bool read)
    {
        auto & timeout = read ? read_timeout : write_timeout;
        timeout += DEFAULT_TIMEOUT_MS;
    }

    uint64_t get_timeout_type(uint64_t current_time) const
    {
        uint64_t type = 0;
        if (current_time >= read_timeout)
            type |= PollerEvent::TIMEOUT_READ;

        if (current_time >= write_timeout)
            type |= PollerEvent::TIMEOUT_WRITE;

        return type;
    }

public:
    std::shared_ptr<Socket> socket;
    uint64_t read_timeout  {INVALID_TIME}; // XXX
    uint64_t write_timeout {INVALID_TIME}; // XXX

};

class Poller
{
public:
    enum Signal
    {
        STOP = 0,
        UPDATE,

    };

private:
    using Sockets      = std::unordered_map<int64_t, SocketContext>;
    using Timeouts     = std::multimap<uint64_t, Sockets::iterator>;
    using EpollEvents  = std::vector<epoll_event>;
    using OutputEvents = std::unordered_set<PollerEvent, Hasher<PollerEvent>>;
    using Removes      = std::vector<std::shared_ptr<Socket>>;

private:
    static const uint64_t MAX_NUMBER_OF_EVENTS = 100;

public:
    Poller();
   ~Poller();

    void update_socket(Socket * socket, uint64_t action, bool add);
    void update_socket(Socket * socket, uint64_t action, bool add, uint64_t timeout);

    void remove_socket(Socket * socket);

    void poll();

    PollerEvent get_next_event();

    void signal(Signal signal)
    {
        eventfd_write(m_signal_fd, signal);
    }

private:
    void add_to_scheduler(Socket * socket, uint64_t action, uint64_t timeout);

private:
    Sockets   m_sockets;
    Scheduler m_scheduler;
    Removes   m_removes;

    SpinlockYield m_lock;

    EpollEvents  m_epoll_events;
    OutputEvents m_out_events;

    Timer m_timer;

    int64_t m_epoll_fd  {-1}; // XXX
    int64_t m_signal_fd {-1}; // XXX

};
