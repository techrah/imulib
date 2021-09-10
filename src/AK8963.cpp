#include "AK8963.hpp"
#include "exceptions.hpp"
#include "serial/util.hpp"

AK8963::AK8963(ISerial *const serial, Logger *const logger)
    : ICSerial(serial, logger)
{
    validateDeviceId(_deviceId, "AK8963");
}

AK8963::~AK8963()
{
}

uint8_t AK8963::whoAmI() const
{
    return _serial->readReg(WIA);
}

void AK8963::startup()
{
    // continuous mode 1
    _serial->writeReg(CNTL1, 0b00010010);
}

void AK8963::shutdown()
{
    // power down
    _serial->writeReg(CNTL1, 0x0);
}

RawValues AK8963::getRawSensorValues()
{
    RawValues v;
    Bytes data = _serial->readReg(ST1, 8);

    v.mx = (uint16_t)data[1] << 8 | data[2];
    v.my = (uint16_t)data[3] << 8 | data[4];
    v.mz = (uint16_t)data[5] << 8 | data[6];

    return v;
}
