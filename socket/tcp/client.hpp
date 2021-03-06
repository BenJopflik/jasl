#pragma once

#include <memory>

#include "socket/socket.hpp"
#include "socket/tcp/tcp.hpp"


class TcpClient : public Tcp
{
public:
    static std::shared_ptr<Socket> create(const Tcp::Params & params);
    ~TcpClient();

protected:
    TcpClient(const Tcp::Params & params);

}; // class TcpClient
