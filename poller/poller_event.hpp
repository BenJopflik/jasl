#pragma once

#include <memory>

#include "../common/hashable.hpp" // XXX
#include "../common/hasher.hpp"

class Socket;

struct PollerEvent : public Hashable<PollerEvent>
{
public:
    enum
    {
        READ  = 1 << 0,
        WRITE = 1 << 1,
        CLOSE = 1 << 2,

        TIMEOUT_READ  = 1 << 3,
        TIMEOUT_WRITE = 1 << 4,

    };

public:
    PollerEvent();
    PollerEvent(const std::shared_ptr<Socket> & socket);
    PollerEvent(const std::shared_ptr<Socket> & socket, uint64_t action);

    operator bool () const {return socket.get() != nullptr;}
    uint64_t hash() const override;
    bool operator == (const PollerEvent & right) const override;

public:
    std::shared_ptr<Socket> socket {nullptr};
    uint64_t                action {0};

};

