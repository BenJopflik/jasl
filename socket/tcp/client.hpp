#pragma once

#include "tcp_socket.hpp"

class TcpClient : public TcpSocket
{
public:
    TcpClient(const std::string & ip_addr, uint64_t port);
    TcpClient(const sockaddr_in & addr);
   ~TcpClient();
};
