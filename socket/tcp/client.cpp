#include "client.hpp"

TcpClient::TcpClient(const std::string & ip_addr, uint64_t port)
{
    m_family_type = AF_INET;
    m_socket_type = SOCK_STREAM;

    create();
    set_reuseaddr(1);
    set_snd_buffer(65536);
    set_rcv_buffer(65536);
    set_nonblock();
    try
    {
        connect(ip_addr, port);
    }
    catch (const std::runtime_error & err)
    {
        close(false);
        throw;
    }

}

TcpClient::TcpClient(const sockaddr_in & addr)
{
    m_family_type = addr.sin_family;
    m_socket_type = SOCK_STREAM;

    create();
    set_reuseaddr(1);
    set_snd_buffer(65536);
    set_rcv_buffer(65536);
    set_nonblock();
    try
    {
        connect(addr);
    }
    catch (const std::runtime_error & err)
    {
        close(false);
        throw;
    }
}

TcpClient::TcpClient(const NewConnection & new_connection)
{
    m_family_type = AF_INET;
    m_socket_type = SOCK_STREAM;
    m_fd = new_connection.fd;
    set_snd_buffer(65536);
    set_rcv_buffer(65536);
    set_nonblock();
    // TODO feel other fields
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

TcpClient * create_tcp_client(const NewConnection & new_connection)
{
    return new TcpClient(new_connection);
}

