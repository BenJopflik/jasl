#pragma once
#include <string>
#include <cstdint>

class StringWrapper;

class SubString
{
public:
    SubString();

    SubString(uint64_t begin, uint64_t end);
    void set(uint64_t begin, uint64_t end);

    std::string str(const StringWrapper & source) const;
    std::string substr(const StringWrapper & source, uint64_t offset = 0, uint64_t length = 0) const;

    uint64_t start() const;
    uint64_t length() const;

    operator bool() const;
    uint64_t size() const;

private:
    uint64_t m_begin  {0};
    uint64_t m_end    {0};
    uint64_t m_length {0};

};


