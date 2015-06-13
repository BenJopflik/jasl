#include "tcp_socket.hpp"

TcpSocket::TcpSocket(const std::string & ip_addr, uint64_t port)
{
    m_family_type = AF_INET;
    m_socket_type = SOCK_STREAM;

    create();
    set_reuseaddr(1);
    set_nonblock();
    connect(ip_addr, port);

}

TcpSocket::~TcpSocket()
{

}

