#ifndef __BED28A96_4CA0_43CE_BBE1_1EDBEE33AE40__
#define __BED28A96_4CA0_43CE_BBE1_1EDBEE33AE40__

extern "C"
{
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
}
#include <fcntl.h>
#include <sys/ioctl.h>
#include "../ISerial.hpp"
#include "../exceptions.hpp"
#include "../util.hpp"

namespace serial
{
    class I2C : public ISerial
    {
    private:
        int _fd;

    public:
        I2C(const char *port, uint8_t deviceAddr)
        {
            _fd = open(port, O_RDWR);
            if (_fd < 0)
            {
                // throw SerialException(0, string_format("Failed to initialize I2C serial connumications for port %s", port));
            }
            if (ioctl(_fd, I2C_SLAVE, deviceAddr) < 0)
            {
                // throw SerialException(0, string_format("Failed to initialize I2C serial connumications for device at address 0x%02X", deviceAddr));
            };
        };
        virtual void writeReg(uint8_t reg, uint8_t data)
        {
            i2c_smbus_write_byte_data(_fd, reg, data);
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
        virtual ~I2C(){};

    private:
        void _readReg(uint8_t reg, uint8_t *out, uint8_t count)
        {
            for (unsigned i = 0; i < count; i++)
            {
                int res = i2c_smbus_read_byte_data(_fd, reg + i);
                if (res < 0)
                {
                    // throw SerialException(res, string_format("Serial read error (reg 0x%02X, count %d)", reg, count));
                }
                out[i] = static_cast<uint8_t>(res);
            }
        }
    };
}
#endif
