#ifndef __CD2C37DD_8201_4EF1_B965_D41066DEEF66__
#define __CD2C37DD_8201_4EF1_B965_D41066DEEF66__

#include <stdio.h>
#include <cstdarg>
#include <cstdio>

#include "../ILogger.hpp"

namespace serial
{
    class Logger : public ILogger
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
