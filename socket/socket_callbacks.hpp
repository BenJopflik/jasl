#pragma once


class Socket;

class SocketCallbacks
{
public:
    virtual ~SocketCallbacks() {}

    virtual void on_read(Socket *) = 0;
    virtual void on_write(Socket *) = 0;
    virtual void on_error(Socket *) = 0;

//    virtual void on_accept(Socket *, const NewConnection &) = 0;
    virtual void on_close(Socket *) = 0;
    virtual void on_connected(Socket *) = 0;
    virtual void on_rearm(Socket *) = 0;
};
