#ifndef __pi_I2C_hpp__
#define __pi_I2C_hpp__

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "../ISerial.hpp"
#include "../exceptions.hpp"

namespace serial
{

    class I2C : public ISerial
    {
    private:
        int _fd;

    public:
        I2C(uint8_t deviceId)
        {
            wiringPiSetup();
            _fd = wiringPiI2CSetup(deviceId);
            if (_fd == 1)
            {
                throw new SerialException();
            }
        };
        virtual void writeReg(uint8_t reg, uint8_t data)
        {
            wiringPiI2CWriteReg8(_fd, reg, data);
        }
        virtual void readReg(uint8_t reg, uint8_t *out, uint8_t count)
        {
            for (int i = 0; i < count; i++)
            {
                out[i] = wiringPiI2CReadReg8(_fd, reg + i);
            }
        }
        virtual uint8_t readReg(uint8_t reg)
        {
            return wiringPiI2CReadReg8(_fd, reg);
        }
        virtual Bytes readReg(uint8_t reg, uint8_t count)
        {
            Bytes out(count);
            for (int i = 0; i < count; i++)
            {
                out[i] = wiringPiI2CReadReg8(_fd, reg);
            }
            return out;
        }
        virtual ~I2C(){};
    };
}
#endif
