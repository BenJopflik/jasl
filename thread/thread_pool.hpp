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
    ThreadPool(uint64_t number_of_threads = 0,
               uint64_t worker_task_queue_size = DEFAULT_WORKER_QUEUE_SIZE) : m_n_of_threads(number_of_threads),
                                                                              m_worker_task_queue_size(worker_task_queue_size)
    {
        if (!m_n_of_threads)
        {
            if (!(m_n_of_threads = std::thread::hardware_concurrency()))
                m_n_of_threads = DEFAULT_NUMBER_OF_WORKERS;
        }

        if (!m_worker_task_queue_size)
            m_worker_task_queue_size = DEFAULT_WORKER_QUEUE_SIZE;

        m_workers.resize(m_n_of_threads);

        run();
    }

   ~ThreadPool()
    {
        for (const auto & worker : m_workers)
            worker->stop();
    }

    bool add_task(const std::function<void()> & task)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        bool result = false;
        uint64_t worker_id = m_next_worker;

        do
        {
            result = m_workers[m_next_worker]->add(task);
            m_next_worker = get_next_worker_id(m_next_worker);
            if (m_next_worker == worker_id)
                break;
        }
        while (!result);

        if (!result)
            std::cerr << "no room for task" << std::endl;

        return result;
    }

private:
    void run()
    {
        for (uint64_t i = 0; i < m_n_of_threads; ++i)
        {
            m_workers[i] = Worker::create(m_worker_task_queue_size);
        }
    }

    uint64_t get_next_worker_id(uint64_t current_id) const
    {
        return (current_id + 1) % m_n_of_threads; // simple round robin
    }

private:
    std::vector<std::unique_ptr<Worker>> m_workers;
    std::mutex m_mutex;

    uint64_t m_next_worker {0};
    uint64_t m_n_of_threads {0};
    uint64_t m_worker_task_queue_size {DEFAULT_WORKER_QUEUE_SIZE};

};
