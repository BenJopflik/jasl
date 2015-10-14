#pragma once

#include <memory>

class Socket;
struct SocketContext
{
public:
    enum
    {
        INVALID_TIME = uint64_t(-1),
        DEFAULT_TIMEOUT_MS = 30000,

    };

    struct Timeout
    {
    public:
        void update_timeout(const uint64_t & current_time);
        void update_timeout_step(const uint64_t & step);

    public:
        uint64_t time {INVALID_TIME};
        uint64_t step {INVALID_TIME};

    }; // struct Timeout

public:
    SocketContext(const std::shared_ptr<Socket> & socket);

    uint64_t get_nearest_timeout() const;
    uint64_t get_timeout_type(const uint64_t & current_time) const;

public:
    std::shared_ptr<Socket> socket;

    Timeout read_timeout;
    Timeout write_timeout;

}; // struct SocketContext
