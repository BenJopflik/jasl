#pragma once
#include <atomic>
#include <memory>
#include <cassert>
#include <cstring>
#include <vector>
#include <cassert>
#include <mutex>
#include <condition_variable>

// https://kjellkod.wordpress.com/2012/11/28/c-debt-paid-in-full-wait-free-lock-free-queue/
// single producer single consumer

template <class T, bool RETURN_ON_EMPTY = true>
class SPSCQueue
{

public:
    SPSCQueue(uint64_t max_size) : m_size(max_size + 1)
    {
        assert(max_size);
        m_data.resize(m_size);
    }

    virtual ~SPSCQueue()
    {
    }

    bool empty() const
    {
        return (m_head.load() == m_tail.load());
    }

    bool full() const
    {
        uint64_t head = m_head.load();
        uint64_t tail = m_tail.load();

        return increment(head) == tail;
    }


    bool push(const T & value)
    {
        uint64_t head = m_head.load();
        uint64_t tail = m_tail.load();

        if (increment(head) == tail)
        {
            if (RETURN_ON_EMPTY)
                return false;

            std::unique_lock<std::mutex> lock(m_mutex);
            m_not_full.wait(lock, [this](){return !full();});
        }

        m_data[head] = value;
        m_head.store(increment(head));

        m_not_empty.notify_one();

        return true;
    }

    bool pop(T & value)
    {
        if (empty())
        {
            if (RETURN_ON_EMPTY)
                return false;

            std::unique_lock<std::mutex> lock(m_mutex);
            m_not_empty.wait(lock, [this](){return !empty();});
        }

        uint64_t tail = m_tail.load();

        value = m_data[tail];
        m_tail.store(increment(tail));

        m_not_full.notify_one();

        return true;
    }

private:
    SPSCQueue(const SPSCQueue &) = delete;
    void operator = (const SPSCQueue &) = delete;

    uint64_t increment(uint64_t value) const
    {
        return (value + 1) % m_size;
    }

private:
    std::vector<T> m_data;
    uint64_t m_size {0};

    std::atomic<uint64_t> m_head {0};
    std::atomic<uint64_t> m_tail {0};

    std::mutex m_mutex;
    std::condition_variable m_not_empty;
    std::condition_variable m_not_full;

};
