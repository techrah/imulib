#ifndef __PiLogger_hpp__
#define __PiLogger_hpp__

#include <stdio.h>
#include <cstdarg>
#include <cstdio>

#include "Logger.hpp"

namespace serial
{
    class PiLogger : public Logger
    {
    public:
        void log(const char *fmt, ...) __attribute__((format(printf, 2, 3)))
        {
            va_list args;
            va_start(args, fmt);
            vprintf(fmt, args);
            va_end(args);
            printf("\r\n");
        }
    };

};

#endif
