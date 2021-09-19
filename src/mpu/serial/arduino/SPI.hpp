#ifndef __B6258875_CA1E_43BE_9378_CC1AA6A049B6__
#define __B6258875_CA1E_43BE_9378_CC1AA6A049B6__

#include <SPI.h>
#include "../ISerial.hpp"
#include "../util.hpp"

namespace serial
{
    class SPI : public ISerial
    {
    protected:
        static const uint8_t READ_FLAG = 0b10000000;
        int _channel;
        uint32_t _speed;
        uint8_t _bitOrder;
        uint8_t _mode;
        SPISettings _settings;

    public:
        SPI(uint8_t channel, uint32_t speed, uint8_t bitOrder, uint8_t mode)
            : _channel(channel), _speed(speed), _bitOrder(bitOrder), _mode(mode)
        {
            _settings = SPISettings(speed, bitOrder, mode);
            pinMode(SS, OUTPUT);
            digitalWrite(SS, HIGH);
            ::SPI.begin();
        }
        virtual void writeReg(uint8_t reg, uint8_t data)
        {
            digitalWrite(SS, LOW);
            ::SPI.beginTransaction(_settings);
            ::SPI.transfer(reg);
            ::SPI.transfer(data);
            ::SPI.endTransaction();
            digitalWrite(SS, HIGH);
        }
        inline virtual uint8_t readReg(uint8_t reg)
        {
            uint8_t tmp;
            _readReg(reg, &tmp, 1);
            return tmp;
        }
        virtual Bytes readReg(uint8_t reg, uint8_t count)
        {
            Bytes out(count);
            _readReg(reg, out.dataBuf(), count);
            return out;
        }
        virtual ~SPI()
        {
            ::SPI.end();
        };

    protected:
        void _readReg(uint8_t reg, uint8_t *out, uint8_t count)
        {
            digitalWrite(SS, LOW);
            ::SPI.beginTransaction(_settings);
            ::SPI.transfer(READ_FLAG | reg);
            ::SPI.transfer(out, count);
            ::SPI.endTransaction();
            digitalWrite(SS, HIGH);
        }
    };
}

#endif
