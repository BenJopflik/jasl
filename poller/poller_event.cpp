#include "poller_event.hpp"

PollerEvent::PollerEvent()
{

}

PollerEvent::PollerEvent(std::shared_ptr<Socket> ptr) : ptr(ptr)
{

}

PollerEvent::PollerEvent(std::shared_ptr<Socket> ptr, uint64_t action) : ptr(ptr), action(action)
{

}

uint64_t PollerEvent::hash() const
{
    return std::hash<decltype(ptr)>()(ptr);
}

bool PollerEvent::operator == (const PollerEvent & right) const
{
    return ptr == right.ptr;
}

