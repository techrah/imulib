#include "SlvSerial.hpp"
#include "serial/util.hpp"

static const char *TAG = "SlvSerial";

const uint8_t SlvSerial::I2C_SLV_ADDR[] = {0x25, 0x28, 0x2B, 0x2E};
const uint8_t SlvSerial::I2C_SLV_REG[] = {0x26, 0x29, 0x2C, 0x2F};
const uint8_t SlvSerial::I2C_SLV_CTRL[] = {0x27, 0x2A, 0x2D, 0x30};
const uint8_t SlvSerial::I2C_SLV_DO[] = {0x63, 0x64, 0x65, 0x66};

SlvSerial::SlvSerial(uint8_t deviceAddr, serial::ISerial *serial, ILogger *const logger)
    : _deviceAddr(deviceAddr),
      _serial(serial),
      _logger(logger),
      _slvConnections(new SlvConnection[sizeof(I2C_SLV_ADDR)])
{
}

SlvSerial::~SlvSerial()
{
    delete _slvConnections;
}

SlvConnection SlvSerial::_addConnection(uint8_t reg, uint8_t count)
{
    uint8_t nextSlvNo = 0;
    ExtSensData nextEsd = EXT_SENS_DATA_00;

    if (_numConn == sizeof(I2C_SLV_ADDR))
    {
        _clearAllConnections();
    }

    if (_numConn > 0)
    {
        SlvConnection last = _slvConnections[_numConn - 1];
        nextSlvNo = last.slvno + 1;
        nextEsd = static_cast<ExtSensData>(last.esd + last.count);
    }

    // save connection
    SlvConnection conn = {nextSlvNo, reg, count, nextEsd};
    _slvConnections[_numConn++] = conn;

    _setSlvForRead(conn);
    return conn;
}

SlvConnection SlvSerial::_getConnection(uint8_t reg, uint8_t count)
{
    for (unsigned i = 0; i < _numConn; i++)
    {
        SlvConnection conn = _slvConnections[i];
        if (conn.reg == reg && conn.count == count)
        {
            return conn;
        }
    }
    return _addConnection(reg, count);
}

void SlvSerial::_clearAllConnections()
{
    for (unsigned i = 0; i < _numConn; i++)
    {
        SlvConnection conn = _slvConnections[i];
        _serial->writeReg(I2C_SLV_ADDR[conn.slvno], _deviceAddr);
        _serial->writeReg(I2C_SLV_REG[conn.slvno], conn.reg);
        _serial->writeReg(I2C_SLV_CTRL[conn.slvno], 0x00); // disable
    }
    _numConn = 0;
}

void SlvSerial::writeReg(uint8_t reg, uint8_t data)
{
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
    _logger->print(TAG, "[new conn] slnvo=");
    _logger->print(NULL, conn.slvno);
    _logger->print(NULL, ", reg=0x");
    _logger->print(NULL, conn.reg, ILogger::Format::hex);
    _logger->print(NULL, ", count=");
    _logger->print(NULL, conn.count);
    _logger->print(NULL, ", esd=0x");
    _logger->println(NULL, conn.esd, ILogger::Format::hex);

    _serial->writeReg(I2C_SLV_ADDR[conn.slvno], I2C_SLV_READ | _deviceAddr);
    _serial->writeReg(I2C_SLV_REG[conn.slvno], conn.reg);
    _serial->writeReg(I2C_SLV_CTRL[conn.slvno], I2C_SLV_EN | conn.count);
    serial::delay(_esdInitialDelay);
}
