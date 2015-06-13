#include "client.hpp"

TcpClient::TcpClient(const std::string & ip_addr, uint64_t port) : TcpSocket(ip_addr, port)
{

}

TcpClient::~TcpClient()
{

}

