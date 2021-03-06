#pragma once

class NonCopyable
{
protected:
    NonCopyable() {}
    ~NonCopyable() {}

private:
    NonCopyable(const NonCopyable &) = delete;
    void operator = (const NonCopyable &) = delete;

}; // class NonCopyable
