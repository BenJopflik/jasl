#pragma once

// https://sites.google.com/site/kjellhedstrom2/active-object-with-cpp0x
// http://www.codeproject.com/Articles/598695/Cplusplus-threads-locks-and-condition-variables

// std::thread wrapper
#include <thread>
#include "memory/spsc_queue.hpp"

class Worker
{
    const std::chrono::microseconds SLEEP_TIME {500};

public:
   ~Worker()
    {
        if (m_thread.joinable())
            m_thread.join();
    }

    static std::unique_ptr<Worker> create(const uint64_t task_queue_size)
    {
        std::unique_ptr<Worker> worker(new Worker(task_queue_size));
        worker->m_thread = std::thread(std::bind(&Worker::run, worker.get()));
        return worker;
    }

    void stop()
    {
        m_tasks.push([=]{stop_();});
    }

    bool add(const std::function<void()> & task)
    {
        return m_tasks.push(task);
    }


private:
    Worker(const Worker &) = delete;
    void operator = (const Worker &) = delete;

    Worker(const uint64_t task_queue_size) : m_tasks(task_queue_size) {}

    void run()
    {
        std::function<void()> func;
        while (!m_done)
        {
            if (m_tasks.pop(func))
                func();
            else
                std::this_thread::sleep_for(SLEEP_TIME);
        }
    }

    void stop_()
    {
        m_done = true;
    }

private:
    SPSCQueue<std::function<void()>> m_tasks;
    std::thread m_thread;
    bool m_done {false};

};
