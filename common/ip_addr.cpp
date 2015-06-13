#include "ip_addr.hpp"

void IpAddr::set(const std::string & ip_, uint16_t port_)
{
   ip = ip_;
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


