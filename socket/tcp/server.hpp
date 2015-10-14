#pragma once

#include <memory>

#include "socket/socket.hpp"
#include "socket/tcp/tcp.hpp"

class TcpServer : public Tcp
{
public:
    static std::shared_ptr<Socket> create(const Tcp::Params & params);
    ~TcpServer();

    void read () override;
    void write() override;

protected:
    TcpServer(const Tcp::Params & params);

}; // class TcpServer
