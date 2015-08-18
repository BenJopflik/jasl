#pragma once

#include <cstdint>

#define ALIGN(x) __declspec(align(x))

struct CacheInfo
{
    int32_t cache_line_size {-1};
    int32_t number_of_cache_lines {-1};
};

CacheInfo get_cache_info();
