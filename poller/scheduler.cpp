#include "scheduler.hpp"


Scheduler::Scheduler()
{

}

void Scheduler::add_event(uint64_t new_timeout, EventPayload payload)
{
    auto timeout = m_timeouts.find(payload);
    if (timeout != m_timeouts.end())
        m_events.erase(timeout->second);
    else
        timeout = m_timeouts.insert(std::make_pair(payload, m_events.end())).first;

    auto event = m_events.insert(std::make_pair(new_timeout, payload));
    timeout->second = event;
}

bool Scheduler::get_next_event(EventPayload & payload, uint64_t current_time)
{
    auto event = m_events.begin();
    if (event == m_events.end() || event->first > current_time)
        return false;

    payload = event->second;
    m_events.erase(event);
    m_timeouts.erase(payload);

    return true;
}

void Scheduler::delete_event(EventPayload payload)
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

    auto first_event = m_events.begin();
    auto current_time = m_timer.milliseconds_from_epoch();
    return (first_event->first <= current_time ? 0 : current_time - first_event->first);
}

