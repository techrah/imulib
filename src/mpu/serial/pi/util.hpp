#ifndef __E5D4C167_6DA5_44BD_A2CE_CBA82DC3519A__
#define __E5D4C167_6DA5_44BD_A2CE_CBA82DC3519A__

#ifdef RASPI

#include <time.h>
#include <chrono>
#include <memory>
#include <stdint.h>

namespace serial
{
    inline void delay(unsigned long ms)
    {
        struct timespec ts;
        ts.tv_sec = ms / 1000;
        ts.tv_nsec = (ms % 1000) * 1e6;
        nanosleep(&ts, NULL);
    }

    inline uint64_t getEpochTimeMs()
    {
        const auto tm = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   tm.time_since_epoch())
            .count();
    }
};

#endif
#endif
