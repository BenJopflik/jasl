#include "cache_info.hpp"

#include <unistd.h>

CacheInfo get_cache_info()
{
    CacheInfo ci;
    ci.cache_line_size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    if (ci.cache_line_size && ci.cache_line_size != -1)
        ci.number_of_cache_lines = sysconf(_SC_LEVEL1_DCACHE_SIZE) / ci.cache_line_size;
    return ci;
}
