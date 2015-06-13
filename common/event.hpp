#pragma once

#include <string>
#include <cstdint>

struct Event
{
    enum
    {
        READ = 1,
        WRITE = 1 << 1,
        REARM = 1 << 2,
        CLOSE = 1 << 3,
        ERROR = 1 << 4,
        STOP  = 1 << 5,

    };

    int64_t  fd {-1};
    uint64_t user_data {0};
    uint64_t mask {0};

};
