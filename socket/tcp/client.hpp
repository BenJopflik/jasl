#pragma once

#include "socket/socket.hpp"

#include <memory>

class TcpClient : public Socket, public std::enable_shared_from_this<Socket>
{
public:
    // TODO move params to tcp_params.hpp
    struct Params
    {
        enum
        {
            REUSE_ADDR = 1 << 0,
            REUSE_PORT = 1 << 1,
            NON_BLOCK  = 1 << 2

        };

        std::string ip {""};
        uint64_t    read_buffer  {0};
        uint64_t    write_buffer {0};
        uint16_t    port  {0};
        uint64_t    flags {0};
    };

public:
    static std::shared_ptr<Socket> create(const Params & params);
    ~TcpClient();

protected:
    TcpClient(const Params & params);

    std::shared_ptr<Socket> get_ptr() override {return shared_from_this();}
};
