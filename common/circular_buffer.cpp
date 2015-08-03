#include "circular_buffer.hpp"

CircularBuffer::CircularBuffer(uint64_t size)
{
    reset(size);
}

CircularBuffer::~CircularBuffer()
{

}

void CircularBuffer::reset(uint64_t size)
{
    m_data.reset(new uint8_t[size]);
    m_size = size;
    m_head = m_data.get();
    m_tail = m_head;

    m_begin = m_head;
    m_end   = m_head + m_size;

    m_overlap = false;
}

uint64_t CircularBuffer::size() const
{
    return (m_overlap ? (m_end - m_tail + m_head - m_begin) : (m_head - m_tail));
}

uint64_t CircularBuffer::capacity() const
{
    return m_size;
}

uint64_t CircularBuffer::available_read() const
{
    return (m_overlap ? m_end - m_tail : m_head - m_tail);
}

uint64_t CircularBuffer::available_write() const
{
    return (m_overlap ? m_tail - m_head : m_end - m_head);
}

bool CircularBuffer::empty() const
{
    return !available_read();
}

bool CircularBuffer::full() const
{
    return !available_write();
}

CircularBuffer::operator uint8_t * ()
{
    return m_head;
}

void CircularBuffer::increase(uint8_t *& ptr, uint64_t size)
{
    if (ptr + size > m_end)
        throw std::out_of_range("");

    ptr += size;
    if (ptr == m_end)
    {
        ptr = m_begin;
        m_overlap = !m_overlap;
    }
}

void CircularBuffer::write(uint64_t size)
{
    if (!size)
        return;

    if (size > available_write())
        throw std::out_of_range("");

    increase(m_head, size);
}

void CircularBuffer::read(uint64_t size)
{
    if (!size)
        return;

    if (size > available_read())
        throw std::out_of_range("");

    increase(m_tail, size);
}

