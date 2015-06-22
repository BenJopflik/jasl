#pragma once

#include <string>
#include <cstdint>

class StringWrapper
{
public:
    StringWrapper(const std::string & source);
    StringWrapper(const char * source, uint64_t size);

    ~StringWrapper();

    operator bool () const;
    uint64_t size() const;

    const char * data() const;
    const char * c_str() const;
    std::string substr(uint64_t pos, uint64_t length) const;

private:
    const char *   m_source {nullptr};
    const uint64_t m_size {0};

};
