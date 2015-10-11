#include "file_descriptor.hpp"

#include <assert.h>
#include <unistd.h>

std::shared_ptr<FileDescriptor> FileDescriptor::create(const FD & fd)
{
    return std::shared_ptr<FileDescriptor>(new FileDescriptor(fd));
}

FileDescriptor::FileDescriptor(const FD & fd) : m_fd(fd)
{
    if (m_fd < 0)
        throw std::runtime_error("Invalid FileDescriptor");
}

FileDescriptor::~FileDescriptor()
{
    if (m_fd != INVALID_FD)
        ::close(m_fd);

    m_fd = INVALID_FD;
}

ssize_t FileDescriptor::write(const uint8_t * data, const ssize_t data_size)
{
    assert( data && data_size && "Invalid input args");

    ssize_t offset  = 0;
    ssize_t n_bytes = 0;

    while ((n_bytes = ::write(m_fd, data + offset, data_size - offset)))
    {
        if (n_bytes <= 0)
        {
            if (errno == EAGAIN || errno == EINTR)
                continue;
        }
        else if ((offset += n_bytes) < data_size)
        {
            continue;
        }

        break;
    }
    return offset;
}

ssize_t FileDescriptor::read(uint8_t * data, const ssize_t data_size, bool & eof)
{
    assert( data && data_size && "invalid input args");

    ssize_t offset  = 0;
    ssize_t n_bytes = 0;

    eof = false;

    while ((n_bytes = ::read(m_fd, data + offset, data_size - offset)))
    {
        if (n_bytes < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
                continue;
        }
        else if ((offset += n_bytes) < data_size)
        {
            continue;
        }

        break;
    }

    if (!n_bytes)
        eof = true;

    return offset;
}
