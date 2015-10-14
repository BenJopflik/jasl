#include "socket/tcp/client.hpp"

#include <unistd.h>
#include <sys/socket.h>

std::shared_ptr<Socket> TcpClient::create(const Params & params)
{
    return std::shared_ptr<TcpClient>(new TcpClient(params));
}

TcpClient::TcpClient(const Params & params) : Socket(SOCK_STREAM, AF_INET)
{
    if (params.flags & Params::NON_BLOCK)
        Socket::set_nonblock();

    if (params.flags & Params::REUSE_ADDR)
        set_reuseaddr(1);

    if (params.read_buffer)
        set_rcvbuf(params.read_buffer);

    if (params.write_buffer)
        set_sndbuf(params.write_buffer);

    try
    {
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
