#pragma once

#include <sys/epoll.h>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "common/file_descriptor.hpp"
#include "poller/poller_event.hpp"
#include "thread/spinlock.hpp"

// TODO add scheduler

class Poller : public std::enable_shared_from_this<Poller>
{
    using ActiveSockets  = std::unordered_map<FileDescriptor::FD, std::shared_ptr<Socket>>;
    using RemovedSockets = std::vector<std::shared_ptr<Socket>>;
    using EpollEvents    = std::vector<epoll_event>;
    using OutputEvents   = std::unordered_set<PollerEvent, Hasher<PollerEvent>>;

public:
    enum Signal
    {
        UPDATE = 1,
        STOP,

    };

public:
    Poller();
   ~Poller();

    void update_socket(const std::shared_ptr<Socket> & socket, uint64_t event_mask);
    void remove_socket(const std::shared_ptr<Socket> & socket);

    void poll();
    PollerEvent get_next_event();
    void signal(const Signal & signal) const;

private:
    uint64_t get_epoll_events();
    void     process_epoll_events(uint64_t number_of_epoll_events);
    void     process_removed_sockets();
// TODO    void     process_scheduler();

    uint64_t process_signal(const uint64_t & signal);

private:
    ActiveSockets  m_active_sockets;
    RemovedSockets m_removed_sockets;
    OutputEvents   m_out_events;

    EpollEvents m_epoll_events;

    SpinlockYield m_lock;
    std::shared_ptr<FileDescriptor> m_epoll_fd;
    std::shared_ptr<FileDescriptor> m_event_fd;

};
