#pragma once

#include <atomic>
#include <thread>

//http://sourceforge.net/p/sobjectizer/repo/903/tree/branches/cppinlocks/1.0/dev/cppinlocks/all.hpp
//http://en.wikipedia.org/wiki/Test_and_Test-and-set

struct BackoffBusy
{
    inline void operator()()
    {

    }
}; // struct BackoffBusy

struct BackoffYield
{
    inline void operator()()
    {
        std::this_thread::yield();
    }
}; // struct BackoffYield

template <class BackoffPolicy>
class Spinlock
{
public :
    Spinlock()
    {

    }

   ~Spinlock()
    {

    }

    void lock()
    {
        BackoffPolicy policy;
        bool expected = false;

        while (!m_lock.compare_exchange_strong(expected, true, std::memory_order_acquire, std::memory_order_relaxed))
        {
            do
            {
                policy();
            }
            while (m_lock.load(std::memory_order_relaxed));

            expected = false;
        }
    }

    void unlock()
    {
        m_lock.store(false, std::memory_order_release);
    }

private:
    Spinlock(const Spinlock &) = delete;
    Spinlock(Spinlock &&)      = delete;

    Spinlock & operator = (const Spinlock &) = delete;
    Spinlock & operator = (Spinlock &&)      = delete;

private:
    std::atomic<bool> m_lock {false};

}; // class Spinlock

using SpinlockBusy  = Spinlock<BackoffBusy>;
using SpinlockYield = Spinlock<BackoffYield>;
