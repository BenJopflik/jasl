#pragma once

#include <functional>
#include <cstdint>

class TimerEvent
{
    using Callback = std::function<void ()>;

public:
    TimerEvent(uint64_t expire_time, Callback cb) : m_expire_time(expire_time), m_cb(cb)
    {
        assert(m_cb);
    }

    bool check(uint64_t now)
    {
        if (now >= m_expire_time)
        {
            m_cb();
            return true;
        }

        return false;
    }

    bool operator < (const TimerEvent & right) const
    {
        return m_expire_time < right.m_expire_time;
    }

private:
    uint64_t m_expire_time {0};
    Callback m_cb {nullptr};

};

