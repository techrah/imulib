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

void AK8963::_changeMode(uint8_t mode)
{
    // Should power down before changing modes
    _serial->writeReg(CNTL1, MODE_POWER_DOWN);

    // At least 100µs required after power down according to specs
    // but in practice, more time is needed.
    serial::delay(100);

    if (mode != MODE_POWER_DOWN)
    {
        _serial->writeReg(CNTL1, mode);
        serial::delay(100);
    }
}

CoordValues<int16_t> AK8963::_xyzBytesToInts(const Bytes &data) const
{
    auto bytesToInt = [](uint8_t hByte, uint8_t lByte)
    {
        return static_cast<int16_t>(static_cast<uint16_t>(hByte) << 8 | lByte);
    };
    return CoordValues<int16_t>({
        bytesToInt(data[1], data[0]),
        bytesToInt(data[3], data[2]),
        bytesToInt(data[5], data[4]),
    });
}

CoordValues<float> AK8963::_getSensitivityMultiplierValues()
{
    _changeMode(MODE_FUSE_ROM_ACCESS);
    Bytes b = _serial->readReg(ASAX, 3);
    _logger->log("ASAX: %u, ASAY: %u, ASAZ: %u", b[0], b[1], b[2]);

    auto calcMultiplier = [](auto val)
    {
        return (val - 128) / 256.0f + 1;
    };

    CoordValues<float> r({
        calcMultiplier(b[0]),
        calcMultiplier(b[1]),
        calcMultiplier(b[2]),
    });
    return r;
}

bool AK8963::selfTest(struct SelfTestResults *out)
{
    auto smv = _getSensitivityMultiplierValues();

    _changeMode(MODE_POWER_DOWN);
    _serial->writeReg(ASTC, SELF);
    _serial->writeReg(CNTL1, BIT_16_BIT_OUTPUT | MODE_SELF_TEST);
    serial::delay(100);
    while ((_serial->readReg(ST1) | DRDY) != DRDY_READY)
    {
    }
    Bytes data = _serial->readReg(HXL, 6);
    _serial->writeReg(ASTC, 0);
    _changeMode(MODE_POWER_DOWN);
    auto values = _xyzBytesToInts(data);

    CoordValues<float> fv(values);
    fv['x'] *= smv['x'];
    fv['y'] *= smv['y'];
    fv['z'] *= smv['z'];

    bool pass = fv['x'] >= -200.0f && fv['x'] <= 200.0f &&
                fv['y'] >= -200.0f && fv['y'] <= 200.0f &&
                fv['z'] >= -3200.0f && fv['z'] <= -800.0f;

    if (out)
    {
        out->sMult = smv;
        out->measurements = values;
        out->adjusted = fv;
        out->pass = pass;
    }

    return pass;
}

void AK8963::startup()
{
    _changeMode(BIT_16_BIT_OUTPUT | MODE_CONTINUOUS_MEASUREMENT_1_8HZ);
}

void AK8963::shutdown()
{
    _changeMode(MODE_POWER_DOWN);
}

CoordValues<int16_t> AK8963::getRawSensorValues()
{
    for (;;)
    {
        for (;;)
        {
            uint8_t st1 = _serial->readReg(ST1);
            // _logger->log("ST1: 0x%02x", st1);
            if ((st1 & DRDY) == DRDY_READY)
            {
                break;
            }
        }
        Bytes data = _serial->readReg(HXL, 7);
        if ((data[6] & HOFL) == HOFL_NORMAL)
        {
            return _xyzBytesToInts(data);
        }
    }
}
