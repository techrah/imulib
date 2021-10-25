#include "MPU65xx.hpp"
#include "serial/util.hpp"
#include "SlvSerial.hpp"

MPU65xx::MPU65xx(ISerial *const serial, ILogger *const logger)
    : ICSerial(serial, logger)
{
    // reset
    // If resetting, must do it now before enabling bypass mode
    // as this will reset the bypass.
    _serial->writeReg(0x6B, 0b10000000);
    serial::delay(10); // needs time to reset
}

bool MPU65xx::startup()
{
    // config accel
    _serial->writeReg(0x1C, 0b00001000); // 4g
    return true;
}

void MPU65xx::shutdown()
{
    // power down
    _serial->writeReg(0x6B, 0b01000000); // SLEEP
}

MPU65xx::~MPU65xx()
{
}

uint8_t MPU65xx::whoAmI() const
{
    return _serial->readReg(0x75);
}

Values<int16_t> MPU65xx::_bytesToInts(const Bytes &data) const
{
    auto bytesToInt = [&data](int16_t value, int ix)
    {
        uint8_t hByte = data[ix * 2];
        uint8_t lByte = data[ix * 2 + 1];
        return static_cast<int16_t>(static_cast<uint16_t>(hByte) << 8 | lByte);
    };

    Values<int16_t> res(3);
    res.apply(bytesToInt);
    return res;
}

Values<int16_t> MPU65xx::getRawAccelSensorValues()
{
    Bytes data = _serial->readReg(0x3B, 6);
    Values<int16_t> values = _bytesToInts(data);
    return values;
}

Values<float> MPU65xx::getAccelSensorValues()
{
    Values<float> values = getRawAccelSensorValues();
    return values * (1.0f / 8192);
}
