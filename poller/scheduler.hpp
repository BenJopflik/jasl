#pragma once

#include <list>
#include <map>
#include <unordered_map>

#include "common/timer.hpp"
#include "poller/event_payload.hpp"

class Scheduler
{
    using Events   = std::multimap<uint64_t, EventPayload>;
    using Timeouts = std::unordered_map<uint64_t, Events::const_iterator>;

public:
    Scheduler();

    void add_event(const uint64_t & timeout, const EventPayload & payload);
    bool get_next_event(EventPayload & payload, const uint64_t & current_time);
    void delete_event(const EventPayload & payload);

    // -1 no events
    //  0 unprocessed events
    // >0 ms to next event
    int64_t ms_to_next_event() const;

private:
    Events   m_events;
    Timeouts m_timeouts;
    Timer    m_timer;

}; // class Scheduler
