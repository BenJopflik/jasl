#pragma once

#include "worker.hpp"
#include <mutex>

class ThreadPool
{
    enum
    {
        DEFAULT_WORKER_QUEUE_SIZE = 10,
        DEFAULT_NUMBER_OF_WORKERS = 1,

    };

public:
    ThreadPool(uint64_t number_of_threads = 0, uint64_t worker_task_queue_size = DEFAULT_WORKER_QUEUE_SIZE);
   ~ThreadPool();

    bool add_task(const std::function<void()> & task);

private:
    void run();
    uint64_t get_next_worker_id(uint64_t current_id) const;

private:
    std::vector<std::unique_ptr<Worker>> m_workers;
    std::mutex m_mutex;

    uint64_t m_next_worker {0};
    uint64_t m_n_of_threads {0};
    uint64_t m_worker_task_queue_size {DEFAULT_WORKER_QUEUE_SIZE};

}; // class ThreadPool
