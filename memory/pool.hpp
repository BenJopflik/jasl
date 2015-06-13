#pragma once
#include "chunker.hpp"

template <class T, uint64_t max_number_of_elements>
class Pool
{
    enum
    {
        SIZEOF_T = sizeof(T),

    };

    using Deleter = std::function<void(T*)>;

public:
    template <typename ...Args>
    std::shared_ptr<T> get_shared(Args... args);

private:
    Chunker<SIZEOF_T, max_number_of_elements> m_chunker;

}; // class Pool


template <class T, uint64_t max_number_of_elements>
template <typename ...Args>
std::shared_ptr<T> Pool<T, max_number_of_elements>::get_shared(Args... args)
{
    auto out = m_chunker.get_shared(sizeof(T));
    if (out)
        return std::shared_ptr<T>(new (out.get()) T(args...), [out](T * ptr){ptr->~T();});
    return std::shared_ptr<T>();
}
