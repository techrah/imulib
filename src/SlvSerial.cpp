#include <algorithm>
#include "SlvSerial.hpp"
#include "serial/util.hpp"

static const char *TAG = "SlvSerial";

const uint8_t SlvSerial::I2C_SLV_ADDR[] = {0x25, 0x28, 0x2B, 0x2E};
const uint8_t SlvSerial::I2C_SLV_REG[] = {0x26, 0x29, 0x2C, 0x2F};
const uint8_t SlvSerial::I2C_SLV_CTRL[] = {0x27, 0x2A, 0x2D, 0x30};
const uint8_t SlvSerial::I2C_SLV_DO[] = {0x63, 0x64, 0x65, 0x66};

SlvSerial::SlvSerial(uint8_t deviceAddr, serial::ISerial *serial, ILogger *logger)
    : _deviceAddr(deviceAddr), _serial(serial), _logger(logger)
{
}

SlvConnection SlvSerial::_addConnection(uint8_t reg, uint8_t count)
{
    uint8_t nextSlvNo = 0;
    ExtSensData nextEsd = EXT_SENS_DATA_00;

    if (_slvConnections.size() == sizeof(I2C_SLV_ADDR))
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
    SlvConnection conn = {nextSlvNo, reg, count, nextEsd};
    _slvConnections.push_back(conn);

    _setSlvForRead(conn);

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
    _logger->log(TAG, "[write] reg=0x%02x, data=0x%02x", reg, data);
    // Use SLV4 since it doesn't affect EXT_SENS_DATA_nn registers
    _serial->writeReg(I2C_SLV4_ADDR, _deviceAddr);
    _serial->writeReg(I2C_SLV4_REG, reg);
    _serial->writeReg(I2C_SLV4_DO, data);
    _serial->writeReg(I2C_SLV4_CTRL, I2C_SLV_EN);
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

void SlvSerial::_setSlvForRead(const SlvConnection &conn)
{
    _logger->log(TAG, "[read] slvno=%d, reg=0x%02x, count=%d, esd=0x%02x", conn.slvno, conn.reg, conn.count, conn.esd);
    _serial->writeReg(I2C_SLV_ADDR[conn.slvno], I2C_SLV_READ | _deviceAddr);
    _serial->writeReg(I2C_SLV_REG[conn.slvno], conn.reg);
    _serial->writeReg(I2C_SLV_CTRL[conn.slvno], I2C_SLV_EN | conn.count);
    serial::delay(_esdInitialDelay);
}
