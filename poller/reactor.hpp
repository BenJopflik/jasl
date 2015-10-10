#pragma once

#include "poller/poller.hpp"
#include "thread/worker.hpp"

//void create_reactors(uint64_t n);


class Reactor
{
public:
    Reactor();

    void run();
    void stop();

    void signal(Poller::Signal signal);
    void update_socket(Socket * socket, uint64_t action, bool add = false);

private:
    Poller m_poller;
    bool m_stop {false};

};

class ReactorInThread : public Reactor
{
public:
    ReactorInThread();
    bool bind_to(uint64_t cpu_id) {return m_worker->bind_to(cpu_id);}
private:
    std::unique_ptr<Worker> m_worker;
    Poller m_poller;
    bool m_stop {false};

};

