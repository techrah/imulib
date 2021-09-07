#ifndef __C7CFE287_2FC4_4FE9_B986_7D410961CA23__
#define __C7CFE287_2FC4_4FE9_B986_7D410961CA23__

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
