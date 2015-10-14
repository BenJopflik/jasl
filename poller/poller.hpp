#pragma once

#include <sys/epoll.h>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "common/file_descriptor.hpp"
#include "common/timer.hpp"
#include "poller/poller_event.hpp"
#include "thread/spinlock.hpp"
#include "poller/scheduler.hpp"
#include "poller/socket_context.hpp"

class Poller : public std::enable_shared_from_this<Poller>
{
    using ActiveSockets  = std::unordered_map<FileDescriptor::FD, SocketContext>;
    using EpollEvents    = std::vector<epoll_event>;
    using OutputEvents   = std::unordered_set<PollerEvent, Hasher<PollerEvent>>;

public:
    enum Signal
    {
        STOP = 0xffffffffffff,
        UPDATE = 1,

    };

public:
    static std::shared_ptr<Poller> create();
   ~Poller();

    void remove_socket(const std::shared_ptr<Socket> & socket);
    void update_socket(const std::shared_ptr<Socket> & socket,
                       const uint64_t & event_mask,
                       const uint64_t & timeout_step = SocketContext::DEFAULT_TIMEOUT_MS);

    void poll();
    PollerEvent get_next_event();

    void signal(const Signal & signal) const;

private:
    Poller();
    uint64_t get_epoll_events();
    void     process_epoll_events(uint64_t number_of_epoll_events);
    void     process_scheduler();

    uint64_t process_signal(const uint64_t & signal);
    void add_to_scheduler(SocketContext  & context,
                          const uint64_t & action,
                          const uint64_t & timeout);

private:
    ActiveSockets  m_active_sockets;
    OutputEvents   m_out_events;
    Scheduler      m_scheduler;
    Timer          m_timer;

    EpollEvents m_epoll_events;

    SpinlockYield m_lock;
    std::shared_ptr<FileDescriptor> m_epoll_fd;
    std::shared_ptr<FileDescriptor> m_event_fd;

}; // class Poller
