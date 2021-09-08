#include "MPU65xx.hpp"
#include "serial/util.hpp"
#include "SlvSerial.hpp"

MPU65xx::MPU65xx(ISerial *const serial, Logger *const logger)
    : ICSerial(serial, logger)
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
}

void MPU65xx::shutdown()
{
    // power down
    _serial->writeReg(0x6B, 0b01000000); // SLEEP
}

MPU65xx::~MPU65xx()
{
}

uint8_t MPU65xx::whoAmI()
{
    return _serial->readReg(0x75);
}

RawValues MPU65xx::getRawSensorValues()
{
    return RawValues();
}
