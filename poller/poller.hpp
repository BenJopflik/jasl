#pragma once
#include <sys/epoll.h>
#include <set>
#include <functional>
#include <cassert>
#include <unordered_map>

#include "memory/spsc_queue.hpp"
#include "socket/socket_base.hpp"
#include "thread/worker.hpp"
#include "common/event.hpp"

struct Action
{
    enum
    {
        DELETE_MASK = ~(uint64_t(0)),

    };

public:
    Action()
    {}

    Action(uint64_t mask, uint64_t timeout, uint64_t user_data) :
                                     mask(mask | EPOLLHUP | EPOLLRDHUP),
                                     timeout_in_milliseconds(timeout),
                                     user_data(user_data)
                                     {}

public:
    uint64_t mask {EPOLLHUP | EPOLLRDHUP};
    uint64_t timeout_in_milliseconds {0};
    uint64_t user_data {0};

};



class Poller
{
public:
    struct Params
    {
        uint64_t poll_interval {0}; // in milliseconds
        uint64_t queue_size {1}; //
    };

public:
    explicit Poller(const Params & params);
    ~Poller();

    bool update(int64_t fd, const Action & action);
    void erase(uint64_t fd);

    bool poll();
    std::unique_ptr<Worker> run_poll_in_thread();

    void stop();

    bool get_event(Event & event);

private:
    void update_();

private:
    Timer m_timer;
    int64_t m_epoll_fd {-1};
    int64_t m_stop_fd  {-1};

    Params m_params;
    std::unique_ptr<epoll_event[]> m_epoll_events;
    uint64_t m_epoll_events_buffer_size {0};

    SPSCQueue<Event> m_events;
    SPSCQueue<std::pair<int64_t, Action>> m_updates; // TODO change to MPSCQueue
//    MPSCQueue<std::pair<int64_t, Action>> m_updates;

    std::unordered_map<int64_t, Action> m_active_sockets;
//    std::multiset<TimerEvent> m_delyed_actions;

};
