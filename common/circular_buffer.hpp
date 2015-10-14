#pragma once

#include <memory>
#include <iostream>

class CircularBuffer
{
public:
    CircularBuffer(uint64_t size);
    ~CircularBuffer();

    void reset(uint64_t size);
    operator uint8_t * ();
    operator const uint8_t * () const;

    void write(uint64_t size);
    void read (uint64_t size);

    inline uint64_t size()            const;
    inline uint64_t capacity()        const;
    inline uint64_t available_read()  const;
    inline uint64_t available_write() const;

    inline bool empty() const;
    inline bool full () const;

private:
    void increase(uint8_t *& ptr, uint64_t size);

private:
    std::unique_ptr<uint8_t[]> m_data;
    uint64_t m_size {0};

    uint8_t * m_head {nullptr};
    uint8_t * m_tail {nullptr};

    uint8_t * m_begin {nullptr};
    uint8_t * m_end   {nullptr};
    bool m_overlap {false};

}; // class CircularBuffer
