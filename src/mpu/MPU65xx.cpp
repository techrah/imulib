#include "MPU65xx.hpp"
#include "serial/util.hpp"
#include "SlvSerial.hpp"

MPU65xx::MPU65xx(ISerial *const serial, const ILogger *const logger)
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
