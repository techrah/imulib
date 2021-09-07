#include "MPU65xx.hpp"
#include "serial/util.hpp"

const uint8_t MPU65xx::I2C_SLV_ADDR[] = {0x25, 0x28, 0x2B, 0x2E, 0x31};
const uint8_t MPU65xx::I2C_SLV_REG[] = {0x26, 0x29, 0x2C, 0x2F, 0x32};
const uint8_t MPU65xx::I2C_SLV_CTRL[] = {0x27, 0x2A, 0x2D, 0x30, 0x33};
const uint8_t MPU65xx::I2C_SLV_DO[] = {0x63, 0x64, 0x65, 0x66};

MPU65xx::MPU65xx(ISerial *const serial, Logger *const logger) : ICSerial(serial, logger)
{
    // WHO_AM_I
    uint8_t mpuId = whoAmI();
    _logger->log("WHO_AM_I: %#02x", mpuId);

    // reset
    // If resetting, must do it now before enabling bypass mode
    // as this will reset the bypass.
    _serial->writeReg(0x6B, 0b10000000);
    serial::delay(10); // needs time to reset
}

void MPU65xx::startup()
{
    // enable bypass
    _serial->writeReg(0x37, 0x22); // enable bypass (INT_PIN_CFG)
    _serial->writeReg(0x6A, 0x20); // enable master mode (USER_CTL)
    _serial->writeReg(0x24, 0x07); // set clock speed 267 kHz (I2C_MST_CTRL)

    // 0x0C WIA
    uint8_t res;
    readSlv(I2CSlv0, 0x00, EXT_SENS_DATA_00, &res, 1, 100);
    _logger->log("0x0C WIA: %#02x", res);

    // mag 16-bit continuous mode 1 (8Mhz)
    writeSlv(I2CSlv0, 0x0A, 0b00010010); // CNTL1

    // // mag 16-bit continuous mode 2 (100Mhz)
    // writeSlv(I2CSlv0, 0x0A, 0b00010110);

    // Start continuous data flow
    setSlvForRead(I2CSlv0, 0x02, 8);

    // Give time for valid data to flow
    serial::delay(12);
}

void MPU65xx::shutdown()
{
    // disable master mode
    _serial->writeReg(0x6A, 0x00);

    // disable bypass
    _serial->writeReg(0x37, 0x00); // (INT_PIN_CFG)

    // power down
    writeSlv(I2CSlv0, 0x0A, 0x0);        // CNTL1
    _serial->writeReg(0x6B, 0b01000000); // SLEEP
}

MPU65xx::~MPU65xx()
{
}

uint8_t MPU65xx::whoAmI()
{
    return _serial->readReg(0x75);
}

void MPU65xx::setSlvForRead(enum MPU65xx::I2CSlv slv, int reg, int count)
{
    _serial->writeReg(I2C_SLV_ADDR[slv], I2C_SLV_READ | 0x0C);
    _serial->writeReg(I2C_SLV_REG[slv], reg);
    _serial->writeReg(I2C_SLV_CTRL[slv], I2C_SLV_EN | count);
}

void MPU65xx::readSlv(enum I2CSlv slv, int reg, enum ExtSensData esd, uint8_t *out, int count, unsigned long delayMs)
{
    setSlvForRead(slv, reg, count);
    serial::delay(delayMs); // Give registers time to get populated.
    _serial->readReg(esd, out, count);
}

void MPU65xx::writeSlv(enum I2CSlv slv, int reg, uint8_t data)
{
    _serial->writeReg(I2C_SLV_ADDR[slv], 0x0C);
    _serial->writeReg(I2C_SLV_REG[slv], reg);
    _serial->writeReg(I2C_SLV_CTRL[slv], I2C_SLV_EN | 1);
    _serial->writeReg(MPU65xx::I2C_SLV_DO[slv], data);
}

RawValues MPU65xx::getRawSensorValues()
{
    RawValues v;
    uint8_t data[8];

    _serial->readReg(EXT_SENS_DATA_00, data, 8);
    v.mx = (uint16_t)data[1] << 8 | data[2];
    v.my = (uint16_t)data[3] << 8 | data[4];
    v.mz = (uint16_t)data[5] << 8 | data[6];

    return v;
}
