#include <assert.h>

#include "poller/socket_context.hpp"
#include "poller/poller_event.hpp"

SocketContext::SocketContext(const std::shared_ptr<Socket> & socket) : socket(socket)
{

}

uint64_t SocketContext::get_nearest_timeout() const
{
    assert(   read_timeout.time  != INVALID_TIME
           || write_timeout.time != INVALID_TIME);

    return std::min(read_timeout.time, write_timeout.time);
}

uint64_t SocketContext::get_timeout_type(const uint64_t & current_time) const
{
    uint64_t type = 0;
    if (current_time >= read_timeout.time)
        type |= PollerEvent::TIMEOUT_READ;

    if (current_time >= write_timeout.time)
        type |= PollerEvent::TIMEOUT_WRITE;

    return type;
}

void SocketContext::Timeout::update_timeout(const uint64_t & current_time)
{
    time = current_time + step;
}

void SocketContext::Timeout::update_timeout_step(const uint64_t & new_step)
{
    int64_t diff = new_step - step;
    if (time != INVALID_TIME)
        time += diff;
    step = new_step;
}
