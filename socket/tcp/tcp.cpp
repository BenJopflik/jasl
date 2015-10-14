#include <unistd.h>
#include <sys/socket.h>

#include "tcp.hpp"

Tcp::Tcp(const Params & params) : Socket(SOCK_STREAM, AF_INET, params.fd)
{
    if (params.flags & Params::NON_BLOCK)
        Socket::set_nonblock();

    if (params.flags & Params::REUSE_ADDR)
        set_reuseaddr(1);

    if (params.flags & Params::REUSE_PORT)
        set_reuseport(1);

    if (params.read_buffer)
        set_rcvbuf(params.read_buffer);

    if (params.write_buffer)
        set_sndbuf(params.write_buffer);
}

Tcp::~Tcp()
{

}
