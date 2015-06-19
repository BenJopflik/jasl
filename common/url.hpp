#pragma once
#include <string>
#include <cassert>
#include "common/sub_string.hpp"

class Url
{
public:
    Url(const std::string & url);
    void set(const std::string & url);
    void print() const;

    operator bool () const {return m_valid;}

    std::string url()   const {return m_url;}
    std::string proto() const {return m_proto.str(m_url);}
    std::string login() const {return m_login.str(m_url);}
    std::string pass()  const {return m_pass.str(m_url);}
    std::string host()  const {return m_host.str(m_url);}
    std::string port()  const {return m_port.str(m_url);}
    std::string tail()  const {return m_tail.str(m_url);}

private:
    void parse(const std::string & url);

    bool fill_proto(uint64_t & offset);
    bool fill_port (uint64_t & offset);
    bool fill_login(uint64_t & offset);
    bool fill_pass (uint64_t & offset);
    void fill_host (uint64_t & offset);
    bool fill_tail (uint64_t & offset);

private:
    std::string m_url;

    SubString m_proto;
    SubString m_login;
    SubString m_pass;
    SubString m_host;
    SubString m_port;
    SubString m_tail;

    bool m_valid {false};

};


