#ifndef __B6258875_CA1E_43BE_9378_CC1AA6A049B6__
#define __B6258875_CA1E_43BE_9378_CC1AA6A049B6__

#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <wiringPiSPI.h>
#include "../ISerial.hpp"
#include "../exceptions.hpp"
#include "../util.hpp"

namespace serial
{
    class SPI : public ISerial
    {
    protected:
        static const uint8_t READ_FLAG = 0b10000000;
        int _fd;
        int _channel;

    public:
        SPI(uint8_t channel, uint32_t speed) : _channel(channel)
        {
            _fd = wiringPiSPISetup(channel, speed);
            if (_fd < 0)
            {
                // throw SerialException(0, string_format("Failed to initialize SPI serial connumications for channel %d", channel));
            }
        }
        virtual void setMode(uint8_t mode)
        {
            ioctl(_fd, SPI_IOC_WR_MODE, &mode);
        }
        virtual void writeReg(uint8_t reg, uint8_t data)
        {
            uint8_t tmp[] = {reg, data};
            wiringPiSPIDataRW(_channel, tmp, 2);
        }
        inline virtual uint8_t readReg(uint8_t reg)
        {
            uint8_t tmp[] = {reg, 0};
            _readReg(reg, tmp, 2);
            return tmp[1];
        }
        virtual Bytes readReg(uint8_t reg, uint8_t count)
        {
            Bytes out(count, 1);
            _readReg(reg, out.dataBuf(), count + 1);
            return out;
        }
        virtual ~SPI(){};

    protected:
        void _readReg(uint8_t reg, uint8_t *out, uint8_t count)
        {
            if (count > 0)
            {
                if (count == 1)
                {
                    out[0] = readReg(reg);
                }
                else
                {
                    out[0] = READ_FLAG | reg;
                    wiringPiSPIDataRW(_channel, out, count);
                }
            }
        }
    };
}

#endif
