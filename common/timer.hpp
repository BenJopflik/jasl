#pragma once

#include <chrono>

class Timer
{
#define NOW std::chrono::high_resolution_clock::now()
#define GET_TIME(type) \
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

    uint64_t elapsed_seconds(bool from_epoch = false) const
    {
        GET_TIME(seconds)
    }

    uint64_t elapsed_milliseconds(bool from_epoch = false) const
    {
        GET_TIME(milliseconds)
    }

    uint64_t elapsed_microseconds(bool from_epoch = false) const
    {
        GET_TIME(microseconds)
    }

    uint64_t elapsed_nanoseconds(bool from_epoch = false) const
    {
        GET_TIME(nanoseconds)
    }

private:
    Timer(const Timer &) = delete;
    void operator=(const Timer &) = delete;

private:
    std::chrono::high_resolution_clock::time_point m_start;

};



