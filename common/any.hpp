#pragma once

#include <memory>

class Any
{
public:
    template <class T>
    T* get()
    {
        if (type() == typeid(T))
            return reinterpret_cast<T*>(get_ptr());

        return 0;
    }

    virtual ~Any()
    {}

    virtual const std::type_info & type() const = 0;

protected:
    virtual void * get_ptr() const = 0;

}; // class Any

using AnyPtr = std::shared_ptr<Any>;

template <class T>
class AnyImpl : public Any
{
public:
    static AnyPtr create(const T& data)
    {
        return AnyPtr(new AnyImpl<T>(new T(data)));
    }

    static AnyPtr create(T* data)
    {
        return AnyPtr(new AnyImpl<T>(data));
    }

    static AnyPtr create(const std::shared_ptr<T>& data)
    {
        return AnyPtr(new AnyImpl<T>(data));
    }

private:
    AnyImpl(const std::shared_ptr<T>& data) : m_data(data)
    {}

    AnyImpl(T* data) : m_data(data)
    {}

    virtual const std::type_info & type() const override
    {
        static const std::type_info& t = typeid(T);
        return t;
    }

    virtual void * get_ptr() const override
    {
        return m_data.get();
    }

private:
    std::shared_ptr<T> m_data;

}; // class AnyImpl

inline AnyPtr create_any(const char * cstr)
{
    return AnyImpl<std::string>::create(new std::string(cstr));
}

template <class T>
inline AnyPtr create_any(const T & data)
{
    return AnyImpl<T>::create(data);
}

template <class T>
inline AnyPtr create_any(T * data)
{
    return AnyImpl<T>::create(data);
}

template <class T>
inline AnyPtr create_any(const std::shared_ptr<T> & data)
{
    return AnyImpl<T>::create(data);
}
