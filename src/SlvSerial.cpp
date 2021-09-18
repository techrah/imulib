#include <algorithm>
#include "SlvSerial.hpp"
#include "serial/util.hpp"

static const char *TAG = "SlvSerial";

const uint8_t SlvSerial::I2C_SLV_ADDR[] = {0x25, 0x28, 0x2B, 0x2E, 0x31};
const uint8_t SlvSerial::I2C_SLV_REG[] = {0x26, 0x29, 0x2C, 0x2F, 0x32};
const uint8_t SlvSerial::I2C_SLV_CTRL[] = {0x27, 0x2A, 0x2D, 0x30, 0x34};
const uint8_t SlvSerial::I2C_SLV_DO[] = {0x63, 0x64, 0x65, 0x66, 0x33};

SlvSerial::SlvSerial(uint8_t deviceAddr, serial::ISerial *serial, ILogger *logger)
    : _deviceAddr(deviceAddr), _serial(serial), _logger(logger)
{
}

SlvConnection SlvSerial::_addConnection(uint8_t reg, uint8_t count)
{
    uint8_t nextSlvNo = 0;
    ExtSensData nextEsd = EXT_SENS_DATA_00;

    if (_slvConnections.size() == 4)
    {
        _clearAllConnections();
    }

    if (!_slvConnections.empty())
    {
        SlvConnection last = _slvConnections.back();
        nextSlvNo = last.slvno + 1;
        nextEsd = static_cast<ExtSensData>(last.esd + last.count);
    }

    // save connection
    _logger->log(TAG, "[add slv] slvno=%d, reg=0x%02x, count=%d, esd=0x%02x", nextSlvNo, reg, count, nextEsd);
    SlvConnection conn = {nextSlvNo, reg, count, nextEsd};
    _slvConnections.push_back(conn);

    setSlvForRead(conn.slvno, nextEsd, reg, count);

    return conn;
}

SlvConnection SlvSerial::_getConnection(uint8_t reg, uint8_t count)
{
    auto is_match = [reg, count](SlvConnection c)
    { return c.reg == reg && c.count == count; };

    auto conn = std::find_if(_slvConnections.begin(), _slvConnections.end(), is_match);
    {
        if (conn != std::end(_slvConnections))
        {
            return *conn;
        }
        return _addConnection(reg, count);
    }
}

void SlvSerial::_clearAllConnections()
{
    for (auto &conn : _slvConnections)
    {
        _serial->writeReg(I2C_SLV_ADDR[conn.slvno], _deviceAddr);
        _serial->writeReg(I2C_SLV_REG[conn.slvno], conn.reg);
        _serial->writeReg(I2C_SLV_CTRL[conn.slvno], 0x00); // disable
    }
    _slvConnections.clear();
}

void SlvSerial::writeReg(uint8_t reg, uint8_t data)
{
    // Use SLV4 since it doesn't affect EXT_SENS_DATA_nn registers
    _serial->writeReg(I2C_SLV4_ADDR, _deviceAddr);
    _serial->writeReg(I2C_SLV4_REG, reg);
    _serial->writeReg(I2C_SLV4_CTRL, I2C_SLV_EN);
    _serial->writeReg(I2C_SLV4_DO, data);
}

uint8_t SlvSerial::readReg(uint8_t reg)
{
    return readReg(reg, 1)[0];
}

serial::Bytes SlvSerial::readReg(uint8_t reg, uint8_t count)
{
    SlvConnection conn = _getConnection(reg, count);
    return _serial->readReg(conn.esd, conn.count);
}

void SlvSerial::setSlvForRead(uint8_t slvno, enum SlvSerial::ExtSensData esd, int reg, uint8_t count)
{
    _serial->writeReg(I2C_SLV_ADDR[slvno], I2C_SLV_READ | _deviceAddr);
    _serial->writeReg(I2C_SLV_REG[slvno], reg);
    _serial->writeReg(I2C_SLV_CTRL[slvno], I2C_SLV_EN | count);
    serial::delay(_esdInitialDelay);
}
