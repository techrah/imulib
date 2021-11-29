#include <assert.h>
#include "AK8963.hpp"
#include "serial/util.hpp"

static const char *TAG = "AK8963";

AK8963::AK8963(ISerial *const serial, ILogger *const logger)
    : ICSerial(serial, logger)
{
}

AK8963::~AK8963()
{
    shutdown();
    delete _sensitivity;
}

void AK8963::setConfig(const struct Config &config)
{
    assert(_currentMode == MODE_POWER_DOWN);
    if (_sensitivity)
    {
        delete _sensitivity;
        _sensitivity = nullptr;
    }
    _config = config;
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

Vector<3, int16_t> AK8963::_bytesToInts(const Bytes &data) const
{
    auto bytesToInt = [&data](int16_t value, int ix)
    {
        uint8_t hByte = data[ix * 2 + 1];
        uint8_t lByte = data[ix * 2];
        return static_cast<int16_t>(static_cast<uint16_t>(hByte) << 8 | lByte);
    };

    Values<int16_t> res(3);
    res.apply(bytesToInt);
    return res.data();
}

void AK8963::_retrieveSensitivityValues()
{
    _changeMode(MODE_FUSE_ROM_ACCESS);
    Bytes b = _serial->readReg(ASAX, 3);
    _config.asa = b.dataBuf();

    _logger->print(TAG, "ASAX:");
    _logger->print(NULL, _config.asa(0));
    _logger->print(NULL, ", ASAY:");
    _logger->print(NULL, _config.asa(1));
    _logger->print(NULL, ", ASAZ:");
    _logger->println(NULL, _config.asa(2));
}

void AK8963::_computeSensitivityMultipliers()
{
    static const auto calcMultiplier = [this](Matrix<1, 3, Reference<Array<3, 3, float>>> row, int ix)
    {
        // AK8963 p32
        row(0, ix) = (_config.asa(ix) - 128) / 256.0f + 1;
    };

    if (!_sensitivity)
    {
        _sensitivity = new Matrix<3, 3>(Zeros<3, 3>());
    }

    _sensitivity->rowApply(calcMultiplier);

    _logger->print(TAG, "Sensitivity multipliers: mx=");
    _logger->print(NULL, (*_sensitivity)(0, 0));
    _logger->print(NULL, ", my=");
    _logger->print(NULL, (*_sensitivity)(1, 1));
    _logger->print(NULL, ", mz=");
    _logger->println(NULL, (*_sensitivity)(2, 2));
}

bool AK8963::selfTest(struct SelfTestResults *out)
{
    static const float normalRanges14[] = {-50, 50, -50, 50, -800, -200};
    static const float normalRanges16[] = {-200, 200, -200, 200, -3200, -800};

    _retrieveSensitivityValues();
    _computeSensitivityMultipliers();

    _changeMode(MODE_SELF_TEST);
    auto values = getRawSensorValues();
    _changeMode(MODE_POWER_DOWN);

    Vector<3> fv = values;
    fv = *_sensitivity * fv;

    const float *ranges = _bitOutput == BIT_16_BIT_OUTPUT ? normalRanges16 : normalRanges14;
    bool pass = fv(0, 0) >= ranges[0] && fv(0, 0) <= ranges[1] &&
                fv(1, 1) >= ranges[2] && fv(1, 1) <= ranges[3] &&
                fv(2, 2) >= ranges[4] && fv(2, 2) <= ranges[5];

    if (out)
    {
        out->asa = _config.asa;
        out->sMult = _sensitivity->Diag();
        out->measurements = values;
        out->adjusted = fv;
        out->pass = pass;
    }

    return pass;
}

int AK8963::startup(enum CNTL1FlagsMode mode)
{
    static const uint64_t TRY_INTERVAL_MS = 1000;

    assert(mode & (MODE_SINGLE_MEASUREMENT |
                   MODE_CONTINUOUS_MEASUREMENT_1_8HZ |
                   MODE_CONTINUOUS_MEASUREMENT_2_100HZ));

    if (_currentMode != MODE_POWER_DOWN)
    {
        return -1;
    }

    if (validateDeviceId(_deviceId, TAG))
    {
        _serial->writeReg(CNTL2, SRST);
        serial::delay(1); // reset causes power down, so delay
    }
    else
    {
        return -2;
    }

    if (!_sensitivity)
    {
        _retrieveSensitivityValues();
        _computeSensitivityMultipliers();
    }

    for (int i = 0; i < 3; i++)
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
                return 0;
            }
            if (getEpochTimeMs() - prevTime > TRY_INTERVAL_MS)
            {
                break;
            }
            serial::delay(10);
        }
    }

    shutdown();
    return -3;
}

void AK8963::shutdown()
{
    _changeMode(MODE_POWER_DOWN);
}

Vector<3, int16_t> AK8963::getRawSensorValues()
{
    assert(_currentMode != MODE_POWER_DOWN);
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
            return _bytesToInts(data);
        }
    }
}

Vector<3, int16_t> AK8963::getSingleRawSensorValues()
{
    startup(MODE_SINGLE_MEASUREMENT);
    return getRawSensorValues();
    // Don't call shutdown(); power down is automatic
}

void AK8963::setBitOutput(enum AK8963::CNTL1FlagsBitOutput bitOutput)
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

Vector<3> AK8963::getSensorValues()
{
    assert(_currentMode != MODE_POWER_DOWN && _sensitivity);
    Vector<3> values = getRawSensorValues();
    values = *_sensitivity * _scaleFactor * values - _config.offset;
    return ~(~values * _config.transform);
}
