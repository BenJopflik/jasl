#pragma once

#include <memory>

#include "socket/socket.hpp"

class Tcp : public Socket, public std::enable_shared_from_this<Socket>
{
public:
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
        uint64_t    flags {0};
        uint64_t    backlog {0};
        int64_t     fd {-1};

        uint16_t    port  {0};
    };

protected:
    Tcp(const Params & params);
   ~Tcp();

    std::shared_ptr<Socket> get_ptr() override {return shared_from_this();}

}; // class Tcp
