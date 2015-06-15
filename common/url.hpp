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

    std::string get_url()   const {return m_url;}
    std::string get_proto() const {return m_proto.get(m_url);}
    std::string get_login() const {return m_login.get(m_url);}
    std::string get_pass()  const {return m_pass.get(m_url);}
    std::string get_host()  const {return m_host.get(m_url);}
    std::string get_port()  const {return m_port.get(m_url);}
    std::string get_tail()  const {return m_tail.get(m_url);}

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


