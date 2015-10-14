#pragma once

// https://sites.google.com/site/kjellhedstrom2/active-object-with-cpp0x
// http://www.codeproject.com/Articles/598695/Cplusplus-threads-locks-and-condition-variables

// std::thread wrapper
#include <thread>
#include <mutex>

#include "common/non_copyable.hpp"
#include "memory/mpsc_queue.hpp"

class Worker : public NonCopyable
{
    const std::chrono::microseconds SLEEP_TIME {500};

public:
   ~Worker();

    static std::unique_ptr<Worker> create(const uint64_t task_queue_size);
    void stop();
    bool add(const std::function<void()> & task);
    bool bind_to(uint64_t cpu_id);

private:
    Worker(const uint64_t task_queue_size);

    void run();
    void stop_();

private:
    SPSCQueue<std::function<void()>> m_tasks;
    std::thread m_thread;
    bool m_done {false};

}; // class Worker
