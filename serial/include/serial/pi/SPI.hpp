#ifndef __B6258875_CA1E_43BE_9378_CC1AA6A049B6__
#define __B6258875_CA1E_43BE_9378_CC1AA6A049B6__

#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <wiringPiSPI.h>
#include "../ISerial.hpp"
#include "../exceptions.hpp"

namespace serial
{
    class SPI : public ISerial
    {
    private:
        int _fd;
        int _channel;

    public:
        SPI(uint8_t channel, uint32_t speed) : _channel(channel)
        {
            _fd = wiringPiSPISetup(channel, speed);
            if (_fd < 0)
            {
                throw SerialException();
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
            readReg(reg, tmp, 2);
            return tmp[1];
        }
        virtual Bytes readReg(uint8_t reg, uint8_t count)
        {
            Bytes out(count, 1);
            readReg(reg, out.dataBuf(), count + 1);
            return out;
        }
        virtual ~SPI(){};

    private:
        void readReg(uint8_t reg, uint8_t *out, uint8_t count)
        {
            if (count == 1)
            {
                out[0] = readReg(reg);
            }
            else
            {
                out[0] = 0x80 | reg;
                wiringPiSPIDataRW(_channel, out, count);
            }
        }
    };
}

#endif
