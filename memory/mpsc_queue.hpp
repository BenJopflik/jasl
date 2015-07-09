#pragma once

#include <mutex>
#include "memory/spsc_queue.hpp"

template <class T, class Lock, bool RETURN_ON_EMPTY = true>
class MPSCQueue : private SPSCQueue<T, RETURN_ON_EMPTY>
{
public:
    MPSCQueue(uint64_t max_size) : SPSCQueue<T>(max_size)
    {

    }

    ~MPSCQueue()
    {

    }

    bool push(const T & value)
    {
        std::lock_guard<Lock> lock(m_lock);
        return SPSCQueue<T>::push(value);
    }

    bool pop(T & value)
    {
        return SPSCQueue<T>::pop(value);
    }

private:
    MPSCQueue(const MPSCQueue &) = delete;
    void operator = (const MPSCQueue &) = delete;

private:
    Lock m_lock;

};
