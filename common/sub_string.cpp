#include "common/sub_string.hpp"
#include <cassert>

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

std::string SubString::str(const std::string & source) const
{
    assert((m_begin + m_length < source.size()));
    return source.substr(m_begin, m_length);
}

std::string SubString::substr(const std::string & source, uint64_t offset, uint64_t length) const
{
    const uint64_t SOURCE_SIZE = source.size();
    const uint64_t SUBSTR_OFFSET = m_begin + offset;

    if (SUBSTR_OFFSET > SOURCE_SIZE || SUBSTR_OFFSET + length > SOURCE_SIZE)
        return "";

    return source.substr(SUBSTR_OFFSET, length);
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

