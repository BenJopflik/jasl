#pragma once

#include <cstdint>

struct SocketInfo
{
    int64_t family;
    int64_t type;

    uint64_t state;
}; // struct SocketInfo
