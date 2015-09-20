#pragma once

#include "socket/socket.hpp"
#include "arpa/inet.h"

class TcpServer;
TcpServer * create_tcp_server(uint64_t port);

class TcpServer : public Socket
{
    friend TcpServer * create_tcp_server(uint64_t port);

private:
    void read() override;
    void write() override;

protected:
    TcpServer(uint64_t port);
    virtual ~TcpServer();
};

