#pragma once

#include "../socket.hpp"
#include "arpa/inet.h"

class TcpSocket : public Socket
{
protected:
    TcpSocket(const sockaddr_in & addr);
    TcpSocket(const std::string & ip_addr, uint64_t port);
    virtual ~TcpSocket();

};
