#include "common/sub_string.hpp"
#include "common/string_wrapper.hpp"
#include <cassert>
#include <stdexcept>

SubString::SubString()
{

}

SubString::SubString(uint64_t begin, uint64_t end)
{
    set(begin, end);
}

void SubString::set(uint64_t begin, uint64_t end)
{
    if (!end)
        end = begin;

    assert(end >= begin);

    m_length = end - begin;
    m_begin = begin;
    m_end = end;
}

std::string SubString::str(const StringWrapper & source) const
{
    return substr(source);
}

std::string SubString::substr(const StringWrapper & source, uint64_t offset, uint64_t length) const
{
    const uint64_t SUBSTR_OFFSET = m_begin + offset;
    const uint64_t SOURCE_SIZE = source.size();

    if (SUBSTR_OFFSET > SOURCE_SIZE)
        throw std::runtime_error("SubString's begin is out of range");

    uint64_t real_size = (length) ? (length) : m_length;
    if (SUBSTR_OFFSET + real_size > SOURCE_SIZE)
        real_size = SOURCE_SIZE - SUBSTR_OFFSET;

    return source.substr(SUBSTR_OFFSET, real_size);
}

uint64_t SubString::start() const
{
    return m_begin;
}

uint64_t SubString::length() const
{
    return m_length;
}

SubString::operator bool () const
{
    return m_length > 0;
}

uint64_t SubString::size() const
{
    return m_length;
}

