#include <iostream>

#include "server.hpp"


std::shared_ptr<Socket> TcpServer::create(const Tcp::Params & params)
{
    return std::shared_ptr<Socket>(new TcpServer(params));
}

TcpServer::TcpServer(const Tcp::Params & params) : Tcp(params)
{
    if (!params.backlog)
        throw std::runtime_error("invalid backlog value");

    Socket::bind(params.ip, params.port);
    Socket::listen(params.backlog);
}

TcpServer::~TcpServer()
{

}

void TcpServer::read()
{
    Socket::accept();
}

void TcpServer::write()
{
    throw std::runtime_error("invalid operation on listening socket");
}
