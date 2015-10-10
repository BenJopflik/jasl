#pragma once

#include <chrono>

class Timer
{
#define NOW std::chrono::high_resolution_clock::now()
#define GET_TIME(type, from_epoch) \
    auto duration = NOW - ((from_epoch) ? decltype(m_start)() : m_start);\
    return std::chrono::duration_cast<std::chrono::type>(duration).count();


public:
    Timer(bool reset_start = false)
    {
        reset();
    }

    void reset()
    {
        m_start = NOW;
    }

    uint64_t elapsed_seconds() const
    {
        GET_TIME(seconds, false)
    }

    uint64_t elapsed_milliseconds() const
    {
        GET_TIME(milliseconds, false)
    }

    uint64_t elapsed_microseconds() const
    {
        GET_TIME(microseconds, false)
    }

    uint64_t elapsed_nanoseconds() const
    {
        GET_TIME(nanoseconds, false)
    }

// from epoch

    uint64_t seconds_from_epoch() const
    {
        GET_TIME(seconds, true)
    }

    uint64_t milliseconds_from_epoch() const
    {
        GET_TIME(milliseconds, true)
    }

    uint64_t microseconds_from_epoch() const
    {
        GET_TIME(microseconds, true)
    }

    uint64_t nanoseconds_from_epoch() const
    {
        GET_TIME(nanoseconds, true)
    }

private:
    Timer(const Timer &) = delete;
    void operator=(const Timer &) = delete;

private:
    std::chrono::high_resolution_clock::time_point m_start;

};



