#ifndef __BED28A96_4CA0_43CE_BBE1_1EDBEE33AE40__
#define __BED28A96_4CA0_43CE_BBE1_1EDBEE33AE40__

#include <Wire.h>
#include "../ISerial.hpp"

namespace serial
{
    class I2C : public ISerial
    {
    private:
        uint8_t _addr;

    public:
        I2C(uint8_t deviceAddr) : _addr(deviceAddr)
        {
            Wire.begin();
            Wire.setClock(400000);
        };
        virtual void writeReg(uint8_t reg, uint8_t data)
        {
            Wire.beginTransmission(_addr);
            Wire.write(reg);
            Wire.write(data);
            Wire.endTransmission();
        }
        virtual uint8_t readReg(uint8_t reg)
        {
            uint8_t res;
            _readReg(reg, &res, 1);
            return res;
        }
        virtual Bytes readReg(uint8_t reg, uint8_t count)
        {
            Bytes out(count);
            _readReg(reg, out.dataBuf(), count);
            return out;
        }
        virtual ~I2C()
        {
            Wire.end();
        };

    private:
        void _readReg(uint8_t reg, uint8_t *out, uint8_t count)
        {
            Wire.beginTransmission(_addr);
            Wire.write(reg);
            Wire.endTransmission(false);
            if (Wire.requestFrom(_addr, count) == count)
            {
                for (unsigned i = 0; i < count; i++)
                {
                    out[i] = Wire.read();
                }
            }
        }
    };
}
#endif
