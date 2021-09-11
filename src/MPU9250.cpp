#include "MPU9250.hpp"
#include "serial/util.hpp"
#include "SlvSerial.hpp"
#include "exceptions.hpp"

MPU9250::MPU9250(ISerial *const serial, I2C *const auxSerial, Logger *const logger)
    : MPU65xx(serial, logger), _auxSerial(auxSerial)
{
    validateDeviceId(_deviceId, "MPU-9250");
}

MPU9250::~MPU9250()
{
}

void MPU9250::startup()
{
    if (_auxSerial)
    {
        _mag = new AK8963(_auxSerial, _logger);
    }
    else
    {
        // enable bypass
        _serial->writeReg(0x37, 0x22); // enable bypass (INT_PIN_CFG)
        _serial->writeReg(0x6A, 0x20); // enable master mode (USER_CTL)
        _serial->writeReg(0x24, 0x07); // set clock speed 267 kHz (I2C_MST_CTRL)

        _mag = new AK8963(new SlvSerial(0x0C, _serial), _logger);
    }
    _mag->startup();
}

void MPU9250::shutdown()
{
    _mag->shutdown();

    // disable master mode
    _serial->writeReg(0x6A, 0x00);

    // disable bypass
    _serial->writeReg(0x37, 0x00); // (INT_PIN_CFG)

    // power down
    _serial->writeReg(0x6B, 0b01000000); // SLEEP
}

uint8_t MPU9250::whoAmI() const
{
    return _serial->readReg(0x75);
}

RawValues MPU9250::getRawSensorValues()
{
    return _mag->getRawSensorValues();
}
