#include "bitset.hpp"
#include <cstring>

Bit::Bit(BitSet & bitset, uint64_t index) : m_bitset(bitset), m_index(index)
{

}

Bit::~Bit()
{

}

Bit & Bit::operator = (const Bit & right)
{
    *this = (bool)right;
    return *this;
}

Bit & Bit::operator = (bool value)
{
    if (value)
        m_bitset.set(m_index);
    else
        m_bitset.clear(m_index);

    return *this;
}

Bit::operator bool () const
{
    return m_bitset.get(m_index);
}

bool Bit::operator ~ () const
{
    return !(bool)*this;
}

bool Bit::flip()
{
    return *this = ~*this;
}

//-----------------------------------------------

BitSet::BitSet()
{

}

BitSet::BitSet(uint64_t size, const uint8_t * source)
{
    bind(size, source);
}

BitSet::BitSet(const BitSet & right)
{
    bind(right.m_size, right.m_data.get());
}

BitSet::~BitSet()
{

}

void BitSet::bind(uint64_t size, const uint8_t * source)
{
    if (!size)
        throw std::runtime_error("BitSet::bind : invalid size"); // TODO mv to __FILE__, __FUNC__

    m_data.reset(new uint8_t[size]);
    m_begin = m_data.get();
    m_size = size;
    m_number_of_bits = m_size * 8;
    if (source)
        memcpy(m_begin, source, size);
    else
        set_all(0);
}

void BitSet::set_all(bool val)
{
    if (!m_data)
        throw std::runtime_error("uninitialized bitset");

    memset(m_begin, (val) ? 0xff : 0x00, m_size);
}

BitSet & BitSet::operator = (const BitSet & right)
{
    bind(right.m_size, right.m_data.get());
    return *this;
}

Bit BitSet::operator [] (uint64_t index)
{
    if (index > m_number_of_bits)
        throw std::out_of_range("invalid index");

    return Bit(*this, index);
}

bool BitSet::operator [] (uint64_t index) const
{
    if (index > m_number_of_bits)
        throw std::out_of_range("invalid index");

    return get(index);
}

struct Offset
{
    Offset(uint64_t byte, uint64_t bit) : byte(byte), bit(bit)
    {

    }

    uint64_t byte {0};
    uint64_t bit  {0};
};

static inline Offset calc_offset(uint64_t index)
{
    return Offset(index / 8, 7 - index % 8);
}

bool BitSet::get(uint64_t index) const
{
    auto offset = calc_offset(index);
    return m_begin[offset.byte] & 1 << offset.bit;
}

void BitSet::set(uint64_t index)
{
    auto offset = calc_offset(index);
    m_begin[offset.byte] |= 1 << offset.bit;
}

void BitSet::clear(uint64_t index)
{
    auto offset = calc_offset(index);
    m_begin[offset.byte] &= ~(1 << offset.bit);
}

uint64_t BitSet::size() const
{
    return m_number_of_bits;
}

std::string BitSet::to_string() const
{
    std::string output;
    output.resize(m_number_of_bits);
    for (uint64_t i = 0; i < m_number_of_bits; ++i)
        output[i] = (*this)[i] + '0';

    return std::move(output);
}
