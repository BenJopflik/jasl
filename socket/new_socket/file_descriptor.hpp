#pragma once
#include <memory>

class FileDescriptor
{
    using FD = int64_t;

    enum
    {
        INVALID_FD = -1,

    };

public:
    static std::shared_ptr<FileDescriptor> create(const FD & fd);

   ~FileDescriptor();

    operator int64_t () const {return m_fd;}

    ssize_t write(const uint8_t * data, const ssize_t data_size);
    ssize_t read (      uint8_t * data, const ssize_t data_size, bool & eof);

// TODO
//    ssize_t writev();
//    ssize_t readv();

protected:
    FileDescriptor(const FD & fd);

private:
    FD m_fd {INVALID_FD};

};
