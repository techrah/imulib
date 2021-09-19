#ifndef __D0226F7D_9F6C_4ECF_84CE_E3A386E47157__
#define __D0226F7D_9F6C_4ECF_84CE_E3A386E47157__

#ifdef ARDUINO

#include <Arduino.h>
#include <stdint.h>

namespace serial
{
    inline void delay(unsigned long ms)
    {
        ::delay(ms);
    }

    inline uint64_t getEpochTimeMs()
    {
        return millis();
    }
};

#endif
#endif
