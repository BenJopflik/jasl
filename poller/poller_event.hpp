#pragma once

#include <memory>

#include "common/hashable.hpp"
#include "common/hasher.hpp"

class Socket;

struct PollerEvent : public Hashable<PollerEvent>
{
public:
    enum
    {
        READ  = 1 << 0,
        WRITE = 1 << 1,
        CLOSE = 1 << 2,
    };

    enum CloseReason
    {
        PEER_SHUTDOWN       = 1 << 0, // EPOLLRDHUP
        UNEXPECTED_SHUTDOWN = 1 << 1, // EPOLLHUP
        SOCKET_ERROR        = 1 << 2, // EPOLLERR

        TIMEOUT_READ        = 1 << 3,
        TIMEOUT_WRITE       = 1 << 4,

    };

public:
    PollerEvent();
    PollerEvent(const std::shared_ptr<Socket> & socket);
    PollerEvent(const std::shared_ptr<Socket> & socket, const uint64_t & action);
    PollerEvent(const std::shared_ptr<Socket> & socket, const uint64_t & action, const uint64_t & close_reason);

    operator bool () const {return socket.get() != nullptr;}

    uint64_t hash() const override;
    bool operator == (const PollerEvent & right) const override;

public:
    std::shared_ptr<Socket> socket {nullptr};
    uint64_t                action {0};
    uint64_t                close_reason {0};

}; // struct PollerEvent
