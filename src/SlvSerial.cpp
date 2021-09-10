#include "SlvSerial.hpp"
#include "serial/util.hpp"

const uint8_t SlvSerial::I2C_SLV_ADDR[] = {0x25, 0x28, 0x2B, 0x2E, 0x31};
const uint8_t SlvSerial::I2C_SLV_REG[] = {0x26, 0x29, 0x2C, 0x2F, 0x32};
const uint8_t SlvSerial::I2C_SLV_CTRL[] = {0x27, 0x2A, 0x2D, 0x30, 0x34};
const uint8_t SlvSerial::I2C_SLV_DO[] = {0x63, 0x64, 0x65, 0x66, 0x33};

SlvSerial::SlvSerial(uint8_t slvno, uint8_t deviceAddr, serial::ISerial *serial)
    : _slvno(slvno), _deviceAddr(deviceAddr), _serial(serial)
{
}

void SlvSerial::writeReg(uint8_t reg, uint8_t data)
{
    _serial->writeReg(I2C_SLV_ADDR[_slvno], _deviceAddr);
    _serial->writeReg(I2C_SLV_REG[_slvno], reg);
    _serial->writeReg(I2C_SLV_CTRL[_slvno], I2C_SLV_EN | 1);
    _serial->writeReg(I2C_SLV_DO[_slvno], data);
}

uint8_t SlvSerial::readReg(uint8_t reg)
{
    return readReg(reg, 1)[0];
}

serial::Bytes SlvSerial::readReg(uint8_t reg, uint8_t count)
{
    if (reg != _esdReg)
    {
        setSlvForRead(_esd, reg, count);
        serial::delay(_esdInitialDelay);
    }
    return _serial->readReg(_esd, count);
}

void SlvSerial::setSlvForRead(enum SlvSerial::ExtSensData esd, int reg, uint8_t count)
{
    _esd = esd;
    _esdReg = reg;
    _serial->writeReg(I2C_SLV_ADDR[_slvno], I2C_SLV_READ | _deviceAddr);
    _serial->writeReg(I2C_SLV_REG[_slvno], reg);
    _serial->writeReg(I2C_SLV_CTRL[_slvno], I2C_SLV_EN | count);
}
