#pragma once
#include <atomic>
#include <memory>
#include <cassert>
#include <cstring>
#include <vector>
#include <cassert>

// https://kjellkod.wordpress.com/2012/11/28/c-debt-paid-in-full-wait-free-lock-free-queue/
// single producer single consumer

template <class T>
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

    bool push(const T & value)
    {
        uint64_t head = m_head.load();
        uint64_t tail = m_tail.load();

        if (increment(head) == tail)
            return false;

        m_data[head] = value;
        m_head.store(increment(head));

        return true;
    }

    bool pop(T & value)
    {
        if (empty())
            return false;

        uint64_t tail = m_tail.load();

        value = m_data[tail];
        m_tail.store(increment(tail));

        return true;
    }

private:
    SPSCQueue(const SPSCQueue &) = delete;
    void operator = (const SPSCQueue &) = delete;

    uint64_t increment(uint64_t value)
    {
        return (value + 1) % m_size;
    }

private:
    std::vector<T> m_data;
    uint64_t m_size {0};

    std::atomic<uint64_t> m_head {0};
    std::atomic<uint64_t> m_tail {0};

};
