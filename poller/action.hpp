#pragma once
#include <sys/epoll.h>

struct Action
{
    enum
    {
        DELETE_MASK = ~(uint64_t(0)),
        NECESARRY_FLAGS = EPOLLHUP | EPOLLRDHUP | EPOLLONESHOT,

    };

public:
    enum
    {
        READ = EPOLLIN | NECESARRY_FLAGS,
        WRITE = EPOLLOUT | NECESARRY_FLAGS,

    };

public:
    Action()
    {}

    Action(uint64_t mask, uint64_t timeout, uint64_t user_data) :
                                     mask(mask),
                                     timeout_in_milliseconds(timeout),
                                     user_data(user_data)
                                     {}

public:
    uint64_t mask {NECESARRY_FLAGS};
    uint64_t timeout_in_milliseconds {0};
    uint64_t user_data {0};

};
