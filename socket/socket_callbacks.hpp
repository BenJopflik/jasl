#pragma once


class Socket;

class SocketCallbacks
{
friend Socket;

protected:
    SocketCallbacks() {}

public:
    virtual ~SocketCallbacks() {}

    virtual void on_read(Socket *) {}
    virtual void on_write(Socket *) {}
    virtual void on_error(Socket *) {}

    virtual void on_accept(Socket *, const NewConnection &) {}
    virtual void on_close(Socket *, int64_t fd) {} // fd - CLOSED fd
    virtual void on_connected(Socket *) {}
    virtual void on_rearm(Socket *) {}
};
