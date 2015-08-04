#pragma once

#include "tcp_socket.hpp"

class TcpClient;
TcpClient * create_tcp_client(const std::string & ip_addr, uint64_t port);
TcpClient * create_tcp_client(const sockaddr_in & addr);

class TcpClient : public TcpSocket
{
friend TcpClient * create_tcp_client(const std::string & ip_addr, uint64_t port);
friend TcpClient * create_tcp_client(const sockaddr_in & addr);

protected:
    TcpClient(const std::string & ip_addr, uint64_t port);
    TcpClient(const sockaddr_in & addr);
    virtual ~TcpClient();
};
