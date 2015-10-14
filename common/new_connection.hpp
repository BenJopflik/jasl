#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdint>

// TODO add ipv6

struct NewConnection
{
public:
    NewConnection() {}
    NewConnection(uint64_t fd, sockaddr_in saddr) : fd(fd), saddr(saddr) {}

public:
    int64_t     fd {-1};
    sockaddr_in saddr;

}; // struct NewConnection
