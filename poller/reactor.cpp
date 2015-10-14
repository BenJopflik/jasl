#include "reactor.hpp"
#include "socket/socket.hpp"
#include <iostream>

Reactor::Reactor()
{
    m_poller.reset(new Poller());
}

void Reactor::run()
{
    while (!m_stop)
    {
        m_poller->poll();

        for (;;)
        {
            auto event = m_poller->get_next_event();
            if (!event.action)
                break;

            if (event.action & PollerEvent::READ)
                event.socket->read();

            if (event.action & PollerEvent::WRITE)
                event.socket->write();

            if (event.action & PollerEvent::CLOSE)
                event.socket->close();
        }
    }
}

void Reactor::stop() const
{
    m_poller->signal(Poller::Signal::STOP);
    m_stop = true;
}

void Reactor::signal(const Poller::Signal & signal) const
{
    m_poller->signal(signal);
}

void Reactor::update_socket(const std::shared_ptr<Socket> & socket, uint64_t action)
{
    m_poller->update_socket(socket, action);
}

// -------------

ReactorInThread::ReactorInThread()
{
    m_worker = std::move(Worker::create(2));
    m_worker->add(std::bind(&ReactorInThread::run, this));
    m_worker->stop();
}

