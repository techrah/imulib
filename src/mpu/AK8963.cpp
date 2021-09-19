#include <assert.h>
#include "AK8963.hpp"
#include "serial/util.hpp"

static const char *TAG = "AK8963";

AK8963::AK8963(ISerial *const serial, ILogger *const logger)
    : ICSerial(serial, logger), _sensitivity(0)
{
}

AK8963::~AK8963()
{
    shutdown();
}

uint8_t AK8963::whoAmI() const
{
    return _serial->readReg(WIA);
}

void AK8963::_changeMode(enum CNTL1FlagsMode mode)
{
    int8_t cntlValue = _bitOutput | mode;

    // Should power down before changing modes (AK8963 p13)
    if (_currentMode != MODE_POWER_DOWN && (mode == MODE_POWER_DOWN || _currentMode != mode))
    {
        if (_currentMode == MODE_SELF_TEST)
        {
            _logger->println(TAG, "Reset ASTC");
            _serial->writeReg(ASTC, 0);
        }
        _logger->println(TAG, "CNTL1: 0x00");
        _serial->writeReg(CNTL1, MODE_POWER_DOWN);
        _currentMode = mode;

        // At least 100µs required after power down (AK8963 p13)
        serial::delay(1);
    }

    if (mode != MODE_POWER_DOWN)
    {
        if (mode == MODE_SELF_TEST)
        {
            _logger->println(TAG, "Set ASTC");
            _serial->writeReg(ASTC, SELF);
        }
        _logger->print(TAG, "CNTL1: 0x");
        _logger->println(NULL, cntlValue, ILogger::Format::hex);
        _serial->writeReg(CNTL1, cntlValue);
        _currentMode = mode;
    }
}

CoordValues<int16_t> AK8963::_xyzBytesToInts(const Bytes &data) const
{
    auto bytesToInt = [](uint8_t hByte, uint8_t lByte)
    {
        return static_cast<int16_t>(static_cast<uint16_t>(hByte) << 8 | lByte);
    };
    CoordValues<int16_t> res(3);
    res[0] = bytesToInt(data[1], data[0]);
    res[1] = bytesToInt(data[3], data[2]);
    res[2] = bytesToInt(data[5], data[4]);
    return res;
}

CoordValues<float> AK8963::_getSensitivityMultiplierValues()
{
    _changeMode(MODE_FUSE_ROM_ACCESS);
    Bytes b = _serial->readReg(ASAX, 3);
    _logger->print(TAG, "ASAX:");
    _logger->print(NULL, b[0]);
    _logger->print(NULL, ", ASAY:");
    _logger->print(NULL, b[1]);
    _logger->print(NULL, ", ASAZ:");
    _logger->println(NULL, b[2]);

    auto calcMultiplier = [](float val)
    {
        // AK8963 p32
        return (val - 128) / 256.0f + 1;
    };

    CoordValues<float> res(3);
    res[0] = calcMultiplier(b[0]);
    res[1] = calcMultiplier(b[1]);
    res[2] = calcMultiplier(b[2]);
    return res;
}

bool AK8963::selfTest(struct SelfTestResults *out)
{
    static const float normalRanges14[] = {-50, 50, -50, 50, -800, -200};
    static const float normalRanges16[] = {-200, 200, -200, 200, -3200, -800};

    auto smv = _getSensitivityMultiplierValues();

    _changeMode(MODE_SELF_TEST);
    auto values = getRawSensorValues();
    _changeMode(MODE_POWER_DOWN);

    CoordValues<float> fv = values;
    fv *= smv;

    const float *ranges = _bitOutput == BIT_16_BIT_OUTPUT ? normalRanges16 : normalRanges14;
    bool pass = fv[0] >= ranges[0] && fv[0] <= ranges[1] &&
                fv[1] >= ranges[2] && fv[1] <= ranges[3] &&
                fv[2] >= ranges[4] && fv[2] <= ranges[5];

    if (out)
    {
        out->sMult = smv;
        out->measurements = values;
        out->adjusted = fv;
        out->pass = pass;
    }

    return pass;
}

bool AK8963::startup(enum CNTL1FlagsMode mode)
{
    static const uint64_t TRY_INTERVAL_MS = 1000;

    assert(mode & (MODE_SINGLE_MEASUREMENT |
                   MODE_CONTINUOUS_MEASUREMENT_1_8HZ |
                   MODE_CONTINUOUS_MEASUREMENT_2_100HZ));

    if (validateDeviceId(_deviceId, TAG))
    {
        _serial->writeReg(CNTL2, SRST);
        serial::delay(1); // reset causes power down, so delay
    }
    else
    {
        return false;
    }

    _sensitivity = _getSensitivityMultiplierValues();

    for (;;)
    {
        uint64_t prevTime = getEpochTimeMs();
        _changeMode(mode);

        // Make sure we get back the data-ready signal else keep trying.
        for (;;)
        {
            uint8_t st1 = _serial->readReg(ST1);
            if ((st1 & DRDY) == DRDY_READY)
            {
                _logger->println(TAG, "Data ready");
                return;
            }
            if (getEpochTimeMs() - prevTime > TRY_INTERVAL_MS)
            {
                break;
            }
        }
    }

    return true;
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

CoordValues<int16_t> AK8963::getSingleRawSensorValues()
{
    startup(MODE_SINGLE_MEASUREMENT);
    return getRawSensorValues();
    // Don't call shutdown(); power down is automatic
}

void AK8963::setBitOutput(enum CNTL1FlagsBitOutput bitOutput)
{
    static float maxMicroTesla = 4912;
    static float maxRange_14bit = 8190;
    static float maxRange_16bit = 32760;

    _bitOutput = bitOutput;

    switch (bitOutput)
    {
    case BIT_14_BIT_OUTPUT:
        _scaleFactor = maxMicroTesla / maxRange_14bit;
        break;
    case BIT_16_BIT_OUTPUT:
        _scaleFactor = maxMicroTesla / maxRange_16bit;
        break;
    default:;
    }
}

CoordValues<float> AK8963::getSensorValues()
{
    CoordValues<float> rawValues = getRawSensorValues();
    return rawValues * _sensitivity * _scaleFactor;
}
