#pragma once
#include <string>

struct IpAddr
{
public:
    void set(const std::string & host, uint16_t port);
    void clear();
    std::string get() const;

public:
    std::string full_addr {""};
    std::string ip {""};
    uint16_t port {0};
};


