#pragma once

#include "../socket.hpp"

class TcpSocket : public Socket
{

public:
    TcpSocket(const std::string & ip_addr, uint64_t port);
    virtual ~TcpSocket();

private:

};
