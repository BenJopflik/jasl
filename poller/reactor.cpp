#include "reactor.hpp"
#include "socket/socket.hpp"
#include <iostream>

Reactor::Reactor()
{

}


void Reactor::run()
{
    while (!m_stop)
    {
        std::cerr << "poll"  << std::endl;
        m_poller.poll();

        for (;;)
        {
            auto event = m_poller.get_next_event();
            if (!event.ptr)
                break;

//            if (event.action & (PollerEvent::TIMEOUT_READ | PollerEvent::TIMEOUT_WRITE)
//            {
//                event.ptr->timeout(event.action & PollerEvent::TIMEOUT_READ,
//                                   event.action & PollerEvent::TIMEOUT_WRITE);
//            }

            if (event.action & PollerEvent::READ)
                event.ptr->read();

            if (event.action & PollerEvent::WRITE)
                event.ptr->write();

            if (event.action & PollerEvent::CLOSE)
                event.ptr->close();

        }
    }
}


void Reactor::stop()
{
    m_poller.signal(Poller::Signal::STOP);
    m_stop = true;
}

void Reactor::update_socket(Socket * socket, uint64_t action, bool add)
{
    m_poller.update_socket(socket, action, add);
//    m_poller.signal(Poller::Signal::UPDATE);
}

void Reactor::signal(Poller::Signal signal)
{
    m_poller.signal(signal);
}

// -------------

ReactorInThread::ReactorInThread()
{
    m_worker = std::move(Worker::create(2));
    m_worker->add(std::bind(&ReactorInThread::run, this));
    m_worker->stop();
}

