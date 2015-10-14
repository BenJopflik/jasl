#pragma once
#include <memory>

class FileDescriptor
{
public:
    enum
    {
        INVALID_FD = -1,

    };

    using FD = int64_t;

public:
    static std::shared_ptr<FileDescriptor> create(const FD & fd);

   ~FileDescriptor();

    inline bool operator == (const uint64_t & right) const {return (uint64_t)m_fd == right;}
    inline operator uint64_t() const {return (uint64_t)m_fd;}
    inline int64_t get_fd()    const {return m_fd;}

    ssize_t write(const uint8_t * data, const ssize_t data_size) const;
    ssize_t read (      uint8_t * data, const ssize_t data_size) const;
    ssize_t read (      uint8_t * data, const ssize_t data_size, bool & eof) const;

// TODO
//    ssize_t writev();
//    ssize_t readv();

protected:
    FileDescriptor(const FD & fd);

private:
    FD m_fd {INVALID_FD};

};
