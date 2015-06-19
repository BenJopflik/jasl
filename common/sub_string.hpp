#pragma once
#include <string>
#include <cstdint>

class SubString
{
public:
    SubString();

    SubString(uint64_t begin, uint64_t end);
    void set(uint64_t begin, uint64_t end);

    std::string str(const std::string & source) const;
    std::string str(const char * source, uint64_t size) const;

    std::string substr(const std::string & source, uint64_t offset, uint64_t length) const;
    std::string substr(const char * source, uint64_t size, uint64_t offset, uint64_t length) const;

    uint64_t start() const;
    uint64_t length() const;

    operator bool() const;
    uint64_t size() const;

private:
    uint64_t m_begin  {0};
    uint64_t m_end    {0};
    uint64_t m_length {0};

};


