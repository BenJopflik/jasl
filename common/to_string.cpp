#include "common/to_string.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "common/event.hpp"

std::string socket_type_to_string(uint64_t type)
{
    switch (type)
    {
        case SOCK_STREAM:
            return "tcp";

        case SOCK_DGRAM:
            return "udp";

        default:
            return "unknown";
    }
}

std::string socket_family_to_string(uint64_t family)
{
    switch (family)
    {
        case AF_INET:
            return "inet";

        case AF_INET6:
            return "inet6";

        case AF_UNIX:
            return "unix";

        default:
            return "unknown";
    }
}

std::string sockaddr_to_string(const sockaddr_in * addr)
{
    char ip[INET_ADDRSTRLEN];
    memset(ip, 0, INET_ADDRSTRLEN);

    std::string out;
    out = inet_ntop(AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN);
    out += ":";
    out += std::to_string(ntohs(addr->sin_port));
    return std::move(out);
}

std::string event_mask_to_string(uint64_t mask)
{
    std::string output;
    if (mask & Event::READ)
        output = "READ";

    if (mask & Event::WRITE)
    {
        if (!output.empty())
            output += "|";

        output += "WRITE";
    }

    if (mask & Event::REARM)
    {
        if (!output.empty())
            output += "|";

        output += "REARM";
    }

    if (mask & Event::CLOSE)
    {
        if (!output.empty())
            output += "|";

        output += "CLOSE";
    }

    if (mask & Event::ERROR)
    {
        if (!output.empty())
            output += "|";

        output += "ERROR";
    }

    return std::move(output);
}

//sockaddr_in fd_to_sockaddr(int64_t fd)
//{
//    sockaddr_in addr;
//    socklen_t addr_size = sizeof(addr);
//    if (::getpeername(fd, (sockaddr *)&addr, &addr_size) < 0)
//    {
//        std::cerr << "getpeername failed : " << strerror(errno) << std::endl;
//        exit(1);
//    }
//
//    return std::move(addr);
//}
