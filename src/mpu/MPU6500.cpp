#include <math.h>
#include "MPU6500.hpp"
#include "serial/util.hpp"
#include "SlvSerial.hpp"

MPU6500::MPU6500(ISerial *const serial, ILogger *const logger)
    : ICSerial(serial, logger)
{
    // reset
    // If resetting, must do it now before enabling bypass mode
    // as this will reset the bypass.
    _serial->writeReg(PWR_MGMT_1, 0b10000000);
    serial::delay(10); // needs time to reset

    setAccelSensitivity(AFS_SEL_16G);
}

void MPU6500::setAccelSensitivity(enum MPU6500::AFS_SEL sensitivity)
{
    _afsSel = sensitivity;
    _aScale = powf(2, sensitivity - 14);
}

bool MPU6500::startup()
{
    // config accel
    _serial->writeReg(ACCEL_CONFIG, _afsSel << 3);
    return true;
}

void MPU6500::shutdown()
{
    // power down
    _serial->writeReg(PWR_MGMT_1, 0b01000000); // SLEEP
}

MPU6500::~MPU6500()
{
}

uint8_t MPU6500::whoAmI() const
{
    return _serial->readReg(WHO_AM_I);
}

Values<int16_t> MPU6500::_bytesToInts(const Bytes &data) const
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

int MPU6500::setAccelDLPF(uint8_t a_dlpf_cfg)
{
    _serial->writeReg(ACCEL_CONFIG_2, a_dlpf_cfg);
    return 0;
}

Values<int16_t> MPU6500::getRawAccelSensorValues()
{
    Bytes data = _serial->readReg(ACCEL_XOUT_H, 6);
    Values<int16_t> values = _bytesToInts(data);
    return values;
}

Values<float> MPU6500::getAccelSensorValues()
{
    Values<float> _values = getRawAccelSensorValues();
    Values<float> values(3);
    values[0] = _values[1];
    values[1] = _values[0];
    values[2] = -_values[2];
    return values * _aScale;
}
