#pragma once

#include <cstdint>

union EventPayload
{
public:
    EventPayload() {}
    EventPayload(uint64_t u64) : u64(u64) {}
    EventPayload(int64_t  i64) : i64(i64) {}
    EventPayload(void  *  ptr) : ptr(ptr) {}

    operator uint64_t () const {return u64;}

public:
    uint64_t u64 {0};
    int64_t  i64;
    void  *  ptr;

};

