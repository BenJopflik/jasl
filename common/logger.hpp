#pragma once

#include <iostream>
#include <fstream>

namespace Logger
{
    static std::ostream & log(std::ostream & out)
    {
        out << std::endl;
        return out;
    }

    template <class T, class ...Argv>
    static std::ostream & log(std::ostream & out, const T t, Argv... argv)
    {
        out << t;
        return log(out, argv...);
    }

}; // struct Logger

#define DEBUG_LOG(...)    do  { Logger::log(std::cerr, "\e[1;39mDEBUG  \e[0m\t", __FILE__, ":", __LINE__, "\t", ##__VA_ARGS__); } while(0);
#define ERROR_LOG(...)    do  { Logger::log(std::cerr, "\e[1;31mERROR  \e[0m\t", __FILE__, ":", __LINE__, "\t", ##__VA_ARGS__); } while(0);
#define INFO_LOG(...)     do  { Logger::log(std::cerr, "\e[1;32mINFO   \e[0m\t", __FILE__, ":", __LINE__, "\t", ##__VA_ARGS__); } while(0);
#define WARNING_LOG(...)  do  { Logger::log(std::cerr, "\e[1;33mWARNING\e[0m\t", __FILE__, ":", __LINE__, "\t", ##__VA_ARGS__); } while(0);

#define FILE_DEBUG_LOG(dest, ...)    do  { Logger::log(dest, "DEBUG  \t", __FILE__, ":", __LINE__, "\t", ##__VA_ARGS__); } while(0);
#define FILE_ERROR_LOG(dest, ...)    do  { Logger::log(dest, "ERROR  \t", __FILE__, ":", __LINE__, "\t", ##__VA_ARGS__); } while(0);
#define FILE_INFO_LOG(dest, ...)     do  { Logger::log(dest, "INFO   \t", __FILE__, ":", __LINE__, "\t", ##__VA_ARGS__); } while(0);
#define FILE_WARNING_LOG(dest, ...)  do  { Logger::log(dest, "WARNING\t", __FILE__, ":", __LINE__, "\t", ##__VA_ARGS__); } while(0);
