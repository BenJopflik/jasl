#include "string_wrapper.hpp"
#include <stdexcept>

StringWrapper::StringWrapper(const std::string & source) : m_source(source.data()),
                                                           m_size(source.size())
{

}

StringWrapper::StringWrapper(const char * source, uint64_t size) : m_source(source),
                                                                   m_size(size)
{

}

StringWrapper::~StringWrapper()
{

}

StringWrapper::operator bool () const
{
    return m_size;
}

uint64_t StringWrapper::size() const
{
    return m_size;
}

const char * StringWrapper::data() const
{
    return m_source;
}

const char * StringWrapper::c_str() const
{
    return m_source;
}

std::string StringWrapper::substr(uint64_t pos, uint64_t length) const
{
    if (pos > m_size)
        throw std::runtime_error("position is out of range");

    uint64_t real_length = (pos + length <= m_size) ? (length) : (m_size - pos);
    return std::string(m_source + pos, real_length);
}
