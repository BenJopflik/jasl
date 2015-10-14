#include "poller/poller_event.hpp"

PollerEvent::PollerEvent()
{

}

PollerEvent::PollerEvent(const std::shared_ptr<Socket> & socket) : socket(socket)
{

}

PollerEvent::PollerEvent(const std::shared_ptr<Socket> & socket,
                         const uint64_t & action)                : socket(socket), action(action)
{

}

PollerEvent::PollerEvent(const std::shared_ptr<Socket> & socket,
                         const uint64_t & action,
                         const uint64_t & close_reason)          : socket(socket), action(action), close_reason(close_reason)
{

}

uint64_t PollerEvent::hash() const
{
    return std::hash<decltype(socket)>()(socket);
}

bool PollerEvent::operator == (const PollerEvent & right) const
{
    return socket == right.socket;
}

