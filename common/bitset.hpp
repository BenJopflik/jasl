#pragma once

#include <memory>
#include <string>

class BitSet;

class Bit
{
    friend BitSet;

public:
   ~Bit ();

    Bit & operator = (const Bit &);
    Bit & operator = (bool);

    operator bool () const;
    bool operator ~ () const;
    bool flip();

private:
    Bit (BitSet &, uint64_t index);

private:
    BitSet & m_bitset;
    const uint64_t m_index {0};

};

class BitSet
{
    friend Bit;

public:
    BitSet();
    BitSet(uint64_t size_in_bytes, const uint8_t * source = nullptr);
    ~BitSet();

    void bind(uint64_t size_in_bytes, const uint8_t * source = nullptr);
    uint64_t size() const;

    Bit operator [] (uint64_t index);
    bool operator [] (uint64_t index) const;

    void set_all(bool = 0);
    std::string to_string() const;

private:
    bool get(uint64_t index) const;
    void set(uint64_t index);
    void clear(uint64_t index);

private:
    std::unique_ptr<uint8_t []> m_data;
    uint8_t * m_begin {nullptr};
    uint64_t m_size {0};
    uint64_t m_number_of_bits {0};

};
