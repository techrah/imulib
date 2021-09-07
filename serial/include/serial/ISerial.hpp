#ifndef __C0BE8C0C_FAF3_4E56_83FF_005A6FF901FE__
#define __C0BE8C0C_FAF3_4E56_83FF_005A6FF901FE__

#include <stdint.h>
#include "Bytes.hpp"

namespace serial
{
    class ISerial
    {
    public:
        virtual ~ISerial() = default;
        virtual void writeReg(uint8_t reg, uint8_t data) = 0;

        // read one byte
        virtual uint8_t readReg(uint8_t reg) = 0;

        // read multiple bytes into buffer (unsafe but more efficient)
        virtual void readReg(uint8_t reg, uint8_t *out, uint8_t count) = 0;

        // read multiple bytes and return Bytes object (safe)
        virtual Bytes readReg(uint8_t reg, uint8_t count) = 0;
    };
};
#endif
