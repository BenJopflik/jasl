#include "server.hpp"
#include "poller/poller.hpp"

TcpServer::TcpServer(uint64_t port)
{
    m_family_type = AF_INET;
    m_socket_type = SOCK_STREAM;

    create();
    set_reuseaddr(1);
    set_reuseport(1);
    set_nonblock();
    try
    {
        bind("", port); // bind on INADDR_ANY
        listen();
    }
    catch (const std::runtime_error & err)
    {
        close(false);
        throw;
    }
}

TcpServer::~TcpServer()
{

}

TcpServer * create_tcp_server(uint64_t port)
{
    return new TcpServer(port);
}

void TcpServer::read()
{
    Socket::accept();
}

void TcpServer::write()
{
    assert(0);
}

