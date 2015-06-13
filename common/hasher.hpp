#pragma once

#include <cstdint>
#include <type_traits>

template <class T>
class Hashable;

template <class T>
class Hasher
{
    static_assert(!std::is_same<Hashable<T>, T>::value, "");
    static_assert(std::is_base_of<Hashable<T>, T>::value, "");

public:
    uint64_t operator () (const T & obj) const
    {
        return obj.hash();
    }
}; // class Hasher
