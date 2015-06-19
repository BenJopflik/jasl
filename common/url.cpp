#include "url.hpp"
#include <iostream>

Url::Url(const std::string & url)
{
    set(url);
}

void Url::set(const std::string & url)
{
    m_valid = false;
    parse(url);
}

void Url::parse(const std::string & url)
{
    m_url = url + '/';
    uint64_t offset = 0;
    while (offset < m_url.size())
    {
        offset = m_url.find_first_of(":/@", offset);
        if (offset == std::string::npos)
        {
//            std::cerr << "invalid url" << std::endl;
            return;
        }

        char c = m_url[offset];
        switch (c)
        {
            case ':':
                if (fill_proto(offset))
                    break;
                else if (fill_port(offset))
                    break;
                else if (fill_login(offset))
                    break;
                else
                    assert(false);

            case '/':
                if (fill_tail(offset))
                {
                    m_valid = true;
                    return;
                }
                else
                    assert(false);

            case '@':
                if (fill_pass(offset))
                    break;
                else
                    assert(false);

            default:
                assert(false);
        }
    }
    m_valid = true;
}

void Url::print() const
{
    if (m_proto)
        std::cerr << "PROTO: " << m_proto.str(m_url) << std::endl;
    if (m_login)
        std::cerr << "LOGIN: " << m_login.str(m_url) << std::endl;
    if (m_pass)
        std::cerr << "PASS: " << m_pass.str(m_url) << std::endl;
    if (m_host)
        std::cerr << "HOST: " << m_host.str(m_url) << std::endl;
    if (m_port)
        std::cerr << "PORT: " << m_port.str(m_url) << std::endl;
    if (m_tail)
        std::cerr << "TAIL: " << m_tail.str(m_url) << std::endl;
}


bool Url::fill_proto(uint64_t & offset)
{
    if (m_proto)
        return false;

    if (offset + 2 >= m_url.length())
        return false;

    const char * current = &m_url[offset];
    if (current[0] == ':' && current[1] == '/' && current[2] == '/')
    {
        m_proto.set(0, offset);
        offset += 3;
        return true;
    }

    return false;
}

bool Url::fill_port(uint64_t & offset)
{
    if (m_port)
        return false;

    uint64_t not_a_digit = m_url.find_first_not_of("1234567890", offset + 1);
    if ((not_a_digit == std::string::npos || m_url[not_a_digit] == '/') && (not_a_digit - offset > 1))
    {
        m_port.set(offset + 1, (not_a_digit == std::string::npos ? m_url.length() : not_a_digit));
        offset = not_a_digit;
        fill_host(offset);
        return true;
    }

    return false;
}

bool Url::fill_login(uint64_t & offset)
{
    if (m_login)
        return false;

    auto delim = m_url.rfind("/", offset);
    if (delim == std::string::npos)
        m_login.set(0, offset);
    else if (m_url[delim - 1] == '/')
        m_login.set(delim + 1, offset);
    else
        return false;

    offset = offset + 1;
    return true;
}

bool Url::fill_pass(uint64_t & offset)
{
    if (m_pass)
        return false;

    if (!m_login)
        return false;

    auto colon = m_url.rfind(":", offset - 1);
    m_pass.set(colon + 1, offset);
    offset += 1;
    return true;
}

void Url::fill_host(uint64_t & offset)
{
    uint64_t host_begin = 0;
    uint64_t host_end = offset;

    if (m_pass)
    {
        host_begin = m_url.rfind("@", offset - 1);
        ++host_begin;
    }
    else if (m_proto)
    {
        host_begin = m_url.find("://");
        host_begin += 3;
    }

    if (m_port)
    {
        host_end = m_url.rfind(":", offset);
    }

    m_host.set(host_begin, host_end);
}

bool Url::fill_tail(uint64_t & offset)
{
    fill_host(offset);
    m_tail.set(offset + 1, m_url.length());

    return true;
}

