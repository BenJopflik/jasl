#include "worker.hpp"
#include <iostream>
#include <stdexcept>

Worker::~Worker()
{
    if (m_thread.joinable())
    {
        m_thread.join();
        std::cerr << "destroy worker" << std::endl;
    }
}

std::unique_ptr<Worker> Worker::create(const uint64_t task_queue_size)
{
    std::unique_ptr<Worker> worker(new Worker(task_queue_size));
    worker->m_thread = std::thread(std::bind(&Worker::run, worker.get()));
    return worker;
}

void Worker::stop()
{
    m_tasks.push([=]{stop_();});
}

bool Worker::add(const std::function<void()> & task)
{
    return m_tasks.push(task);
}

bool Worker::bind_to(uint64_t cpu_id)
{
    cpu_set_t current_cpu_set;

    CPU_ZERO(&current_cpu_set);

    CPU_SET(cpu_id, &current_cpu_set);

    int set_affinity_result = pthread_setaffinity_np(m_thread.native_handle(), sizeof(cpu_set_t), &current_cpu_set);

    return !set_affinity_result;
}

Worker::Worker(const uint64_t task_queue_size) : m_tasks(task_queue_size)
{

}

void Worker::run()
{
    std::function<void()> func;
    while (!m_done)
    {
        if (m_tasks.pop(func))
        {
            try
            {
                func();
            }
            catch (const std::exception & e)
            {
                std::cerr << "exception in worker: " << e.what() << std::endl;
            }
        }
        else
            std::this_thread::sleep_for(SLEEP_TIME);
    }
}

void Worker::stop_()
{
    m_done = true;
}
