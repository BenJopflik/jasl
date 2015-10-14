#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <cassert>

#include "bit_staff.hpp"
// TODO add multithreading support with spinlock

// max_chunk_size - max size in bytes
#define CHUNKER_TEMPLATE template <uint32_t max_chunk_size,\
                                   uint32_t max_chunks,\
                                   uint64_t corrected_chunk_size>

#define CHUNKER_ARGS max_chunk_size, max_chunks, corrected_chunk_size

template <uint32_t max_chunk_size,
          uint32_t max_chunks,
          uint64_t corrected_chunk_size = is_power_of_two(max_chunk_size) ? max_chunk_size : get_next_power_of_two(max_chunk_size)>
class Chunker
{

static_assert(max_chunks, "invalid max_chunks value");

    struct Block
    {
        uint8_t  data[corrected_chunk_size];
        uint32_t next;
    };

public:
    Chunker();
   ~Chunker();

    std::shared_ptr<uint8_t> get_shared(uint64_t size);

private:
    uint8_t * create();
    void      destroy(uint8_t *ptr);

private:
    std::unique_ptr<uint8_t[]> m_raw_data;
    Block   * m_blocks {nullptr};
    Block   * m_next   {nullptr};
}; // class Chunker

#include "chunker.tpp"

#undef CHUNKER_ARGS
#undef CHUNKER_TEMPLATE
