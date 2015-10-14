#include <iostream>

#include "thread_pool.hpp"

ThreadPool::ThreadPool(uint64_t number_of_threads,
                       uint64_t worker_task_queue_size) : m_n_of_threads(number_of_threads),
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

ThreadPool::~ThreadPool()
{
    for (const auto & worker : m_workers)
        worker->stop();
}

bool ThreadPool::add_task(const std::function<void()> & task)
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

void ThreadPool::run()
{
    for (uint64_t i = 0; i < m_n_of_threads; ++i)
    {
        m_workers[i] = Worker::create(m_worker_task_queue_size);
    }
}

uint64_t ThreadPool::get_next_worker_id(uint64_t current_id) const
{
    return (current_id + 1) % m_n_of_threads; // simple round robin
}
