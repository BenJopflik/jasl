#pragma once
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "common/string_wrapper.hpp"

struct IpAddr
{
public:
    IpAddr(const std::string & host, const uint16_t port = 0);
    static sockaddr_in get_sockaddr_in(const StringWrapper & ip, uint16_t port);

    void set(const std::string & host, uint16_t port);
    void set(const sockaddr_in & addr);
    void clear();

public:
    std::string full_addr {""};
    std::string ip        {""};
    uint16_t    port      {0};
    sockaddr_in addr;

}; // class IpAddr
