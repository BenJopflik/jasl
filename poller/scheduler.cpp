#include "poller/scheduler.hpp"

Scheduler::Scheduler()
{

}

void Scheduler::add_event(const uint64_t & new_timeout, const EventPayload & payload)
{
    if (new_timeout == uint64_t(-1))
        return;

    auto timeout = m_timeouts.find(payload);
    if (timeout != m_timeouts.end())
        m_events.erase(timeout->second);
    else
        timeout = m_timeouts.insert(std::make_pair(payload, m_events.end())).first;

    auto event = m_events.insert(std::make_pair(new_timeout, payload));
    timeout->second = event;
}

bool Scheduler::get_next_event(EventPayload & payload, const uint64_t & current_time)
{
    auto event = m_events.begin();
    if (event == m_events.end() || event->first > current_time)
        return false;

    payload = event->second;
    m_events.erase(event);
    m_timeouts.erase(payload);

    return true;
}

void Scheduler::delete_event(const EventPayload & payload)
{
    auto timeouts = m_timeouts.find(payload);
    if (timeouts == m_timeouts.end())
        return;

    m_events.erase(timeouts->second);
    m_timeouts.erase(timeouts);
}

int64_t Scheduler::ms_to_next_event() const
{
    if (m_events.empty())
        return -1;

    auto first_time = m_events.begin()->first;
    auto current_time = m_timer.milliseconds_from_epoch();
    return (first_time <= current_time ? 0 : first_time - current_time);
}

