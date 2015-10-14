#include "ip_addr.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include "common/to_string.hpp"

IpAddr::IpAddr(const std::string & host, const uint16_t port)
{
    set(host, port);
}

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
    memset(&addr, 0, sizeof(addr));
    addr = IpAddr::get_sockaddr_in(ip, port);
}

void IpAddr::set(const sockaddr_in & addr)
{
    full_addr = sockaddr_to_string(addr);
    uint64_t delim = full_addr.rfind(':');
    ip = full_addr.substr(0, delim);
    port = std::stol(full_addr.substr(delim + 1));
    this->addr = addr;
}

void IpAddr::clear()
{
    full_addr = "";
    ip = "";
    port = 0;
    memset(&addr, 0, sizeof(addr));
}

sockaddr_in IpAddr::get_sockaddr_in(const StringWrapper & ip, uint16_t port)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET; // TODO add ipv6
    if (::inet_pton(addr.sin_family, ip.c_str(), &addr.sin_addr) <= 0)
        throw std::runtime_error(std::string("get_sockaddr_in failed. reason: ").append(strerror(errno)));

    addr.sin_port = htons(port);
    return addr;
}
