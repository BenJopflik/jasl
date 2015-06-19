#include "ip_addr.hpp"
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <iostream>

static std::string host_to_ip(const std::string & host)
{
    addrinfo   hints;
    addrinfo * result;

    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */
//    hints.ai_socktype = SOCK_DGRAM;

    int res = 0;

    if ((res = getaddrinfo(host.c_str(), NULL, & hints, & result))) // host, port, hints, result
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res));
        exit(EXIT_FAILURE);
    }

    std::string output;
    output.reserve(32);

    char buf[1000];
    for (auto rp = result; rp != NULL; rp = rp->ai_next)
    {
        if (rp->ai_family == AF_INET6)
        {
//            memset(buf, 0, 1000);
//            auto addr = (sockaddr_in6 *)rp->ai_addr;
//            if (!inet_ntop(AF_INET6, &addr->sin6_addr, buf, sizeof(buf))) // IPv6
//            {
//                std::cerr << strerror(errno) << std::endl;
//                continue;
//            }
//            output.push_back(buf);
        }
        else
        {
            memset(buf, 0, 1000);
            auto * addr = (sockaddr_in *)rp->ai_addr;
            if (!inet_ntop(AF_INET, &addr->sin_addr, buf, sizeof(buf))) // IPv4
            {
                std::cerr << strerror(errno) << std::endl;
                continue;
            }
            output = buf;
            break;
        }
    }

    freeaddrinfo(result);
    return std::move(output);
}


void IpAddr::set(const std::string & host, uint16_t port_)
{
    sockaddr_in sa;
    if (inet_pton(AF_INET, host.c_str(), &(sa.sin_addr)))
        ip = host;
    else
        ip = host_to_ip(host);

    port = port_;
    full_addr = ip + ":" + std::to_string(port);
}

void IpAddr::clear()
{
    full_addr = "";
    ip = "";
    port = 0;
}

std::string IpAddr::get() const
{
    return full_addr;
}


