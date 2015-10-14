#include "reactor.hpp"
#include "socket/socket.hpp"
#include <iostream>

Reactor::Reactor()
{
    m_poller = Poller::create();
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

            try
            {
                if (event.action & PollerEvent::READ)
                    event.socket->read();

                if (event.action & PollerEvent::WRITE)
                    event.socket->write();

                if (event.action & PollerEvent::CLOSE)
                    event.socket->close(event.close_reason);
            }
            catch (const std::exception & e)
            {
#ifdef DEBUG
    std::cerr << "Exception in callback: " << e.what() << std::endl << "Closing socket." << std::endl;
#endif
                event.socket->close();
            }
        }
    }
}

void Reactor::stop() const
{
    m_stop = true;
    m_poller->signal(Poller::Signal::STOP);
}

void Reactor::signal(const Poller::Signal & signal) const
{
    m_poller->signal(signal);
}

void Reactor::update_socket(const std::shared_ptr<Socket> & socket, const uint64_t & action, const uint64_t & timeout_step)
{
    m_poller->update_socket(socket, action, timeout_step);
}

// -------------

ReactorInThread::ReactorInThread()
{
    m_worker = std::move(Worker::create(2));
    m_worker->add(std::bind(&ReactorInThread::run, this));
    m_worker->stop();
}
