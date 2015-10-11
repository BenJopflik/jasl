#pragma once


class Socket;

class SocketCallbacks
{
friend Socket;

protected:
    SocketCallbacks() {}

public:
    virtual ~SocketCallbacks() {}

    virtual void on_read(Socket *)  {}
    virtual void on_write(Socket *) {}
    virtual void on_error(Socket *) {}

//    virtual void on_accept(Socket *, const NewConnection &) {}
    virtual void on_close(Socket *) {}
};
