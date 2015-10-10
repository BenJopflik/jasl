#pragma once

// https://sites.google.com/site/kjellhedstrom2/active-object-with-cpp0x
// http://www.codeproject.com/Articles/598695/Cplusplus-threads-locks-and-condition-variables

// std::thread wrapper
#include <thread>
#include "common/non_copyable.hpp"
#include "memory/spsc_queue.hpp"

class Worker : public NonCopyable
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

    bool bind_to(uint64_t cpu_id)
    {
        cpu_set_t current_cpu_set;

        CPU_ZERO(&current_cpu_set);

        CPU_SET(cpu_id, &current_cpu_set);

        int set_affinity_result = pthread_setaffinity_np(m_thread.native_handle(), sizeof(cpu_set_t), &current_cpu_set);

        return !set_affinity_result;
    }


private:
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
    SPSCQueue<std::function<void()>, false> m_tasks;
    std::thread m_thread;
    bool m_done {false};

};
