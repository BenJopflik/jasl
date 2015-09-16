#pragma once

#include "socket/socket.hpp"
#include "arpa/inet.h"
#include "common/new_connection.hpp"

class TcpClient;
TcpClient * create_tcp_client(const std::string & ip_addr, uint64_t port);
TcpClient * create_tcp_client(const sockaddr_in & addr);
TcpClient * create_tcp_client(const NewConnection & new_connection);

class TcpClient : public Socket
{
    friend TcpClient * create_tcp_client(const std::string & ip_addr, uint64_t port);
    friend TcpClient * create_tcp_client(const sockaddr_in & addr);
    friend TcpClient * create_tcp_client(const NewConnection & new_connection);

protected:
    TcpClient(const std::string & ip_addr, uint64_t port);
    TcpClient(const sockaddr_in & addr);
    TcpClient(const NewConnection & new_connection);
    virtual ~TcpClient();
};
