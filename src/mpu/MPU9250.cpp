#include "MPU9250.hpp"
#include "serial/util.hpp"
#include "SlvSerial.hpp"

MPU9250::MPU9250(
    ISerial *const serial,
    I2C *const auxSerial,
    ILogger *const logger)
    : MPU6500(serial, logger), _auxSerial(auxSerial)
{
    if (!validateDeviceId(_deviceId, "MPU-9250"))
    {
        // TODO: handle error
        return;
    }

    // Reset
    _serial->writeReg(0x6B, 0x80);
    serial::delay(100);

    // TODO: get config from user
    // AK8963::Config config;

    if (_auxSerial)
    {
        _mag = new AK8963(_auxSerial, _logger);
    }
    else
    {
        // enable bypass
        _serial->writeReg(0x37, 0x22); // enable bypass (INT_PIN_CFG)
        _serial->writeReg(0x6A, 0x20); // enable master mode (USER_CTL)
        _serial->writeReg(0x24, 0x0D); // set clock speed 400 kHz (I2C_MST_CTRL)

        _mag = new AK8963(new SlvSerial(0x0C, _serial, _logger), _logger);
    }
}

MPU9250::~MPU9250()
{
}

void MPU9250::setMagConfig(const AK8963::Config &config)
{
    _mag->setConfig(config);
}

void MPU9250::selfTest(struct AK8963::SelfTestResults *magTestResults)
{
    _mag->selfTest(magTestResults);
}

bool MPU9250::startup()
{
    MPU6500::startup();
    return _mag->startup();
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

Matrix<3, 3> MPU9250::getZCorrectionTransform()
{
    Vector<3> aValues = getAccelSensorValues().data();
    // phi = roll, theta = pitch
    float phi = -atan2(aValues(1), aValues(2));
    float theta = -atan2(-aValues(0), (aValues(1) * sin(phi) + aValues(2) * cos(phi)));
    Matrix<3, 3> Rx = {
        1, 0, 0,
        0, cos(phi), sin(phi),
        0, -sin(phi), cos(phi),
    };
    Matrix<3, 3> Ry = {
        cos(theta), 0, -sin(theta),
        0, 1, 0,
        sin(theta), 0, cos(theta),
    };
    return Ry * Rx;
}

Vector<3> MPU9250::getUprightMagValues()
{
    Vector<3> magValues = getMag()->getSensorValues();
    return ~(~magValues * getZCorrectionTransform());
}
