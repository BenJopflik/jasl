#include "socket/tcp/client.hpp"

std::shared_ptr<Socket> TcpClient::create(const Tcp::Params & params)
{
    return std::shared_ptr<Socket>(new TcpClient(params));
}

TcpClient::TcpClient(const Tcp::Params & params) : Tcp(params)
{
    try
    {
        if (params.fd == FileDescriptor::INVALID_FD)
            Socket::connect(params.ip, params.port);
    }
    catch (const std::exception & e)
    {
        if (   errno != EWOULDBLOCK
            && errno != EAGAIN
            && errno != EINTR
            && errno != EINPROGRESS)
            throw;
    }
}

TcpClient::~TcpClient()
{

}
