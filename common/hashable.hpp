#pragma once

#include <cstdint>

template <class T>
class Hashable
{
public:
    virtual ~Hashable() {}

    virtual uint64_t hash() const = 0;
    virtual bool operator == (const T & right) const = 0;
}; // class Hashable
