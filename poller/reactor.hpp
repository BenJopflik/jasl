#pragma once

#include "poller/poller.hpp"
#include "thread/worker.hpp"

class Reactor
{
public:
    Reactor();

    void run();
    void stop() const;

    void signal(const Poller::Signal & signal) const;
    void update_socket(const std::shared_ptr<Socket> & socket, const uint64_t & action, const uint64_t & timeout_step = SocketContext::DEFAULT_TIMEOUT_MS);

private:
    std::shared_ptr<Poller> m_poller;
    mutable bool m_stop {false};

}; // class Reactor

class ReactorInThread : public Reactor
{
public:
    ReactorInThread();
    bool bind_to(uint64_t cpu_id) {return m_worker->bind_to(cpu_id);}

private:
    std::unique_ptr<Worker> m_worker;

}; // class ReactorInThread
