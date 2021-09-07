#ifndef __util_hpp__
#define __util_hpp__

#include <time.h>

namespace serial
{
    inline void delay(unsigned long ms)
    {
        struct timespec ts;
        ts.tv_sec = ms / 1000;
        ts.tv_nsec = (ms % 1000) * 1e6;
        nanosleep(&ts, NULL);
    }
};

#endif
