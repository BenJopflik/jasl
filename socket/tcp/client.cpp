#include "client.hpp"

TcpClient::TcpClient(const std::string & ip_addr, uint64_t port) : TcpSocket(ip_addr, port)
{

}

TcpClient::TcpClient(const sockaddr_in & addr) : TcpSocket(addr)
{

}

TcpClient::~TcpClient()
{

}

TcpClient * create_tcp_client(const std::string & ip_addr, uint64_t port)
{
    return new TcpClient(ip_addr, port);
}

TcpClient * create_tcp_client(const sockaddr_in & addr)
{
    return new TcpClient(addr);
}


