#pragma once

#include <cstdint>

constexpr bool is_power_of_two(uint64_t value)
{
    return value && !(value & (value - 1));
}

//http://stackoverflow.com/questions/21298285/compile-time-recursive-function-to-compute-the-next-power-of-two-of-an-integer
constexpr uint64_t get_next_power_of_two(uint64_t value, unsigned maxb = 64, unsigned curb = 1)
{
    return maxb <= curb
           ? value
           : get_next_power_of_two(((value - 1) | ((value - 1) >> curb)) + 1, maxb, curb << 1);
}


