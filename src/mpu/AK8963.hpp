#ifndef __A65E8222_D7CE_4904_BDB9_41B3E9028F88__
#define __A65E8222_D7CE_4904_BDB9_41B3E9028F88__

#include <stdint.h>
#include "ICSerial.hpp"
#include "Values.hpp"

class AK8963 : public ICSerial
{
public:
    enum Registers : uint8_t
    {
        WIA = 0x00,
        INFO = 0x01,
        ST1 = 0x02,
        HXL = 0x03,
        HXH = 0x04,
        HYL = 0x05,
        HYH = 0x06,
        HZL = 0x07,
        HZH = 0x08,
        ST2 = 0x09,
        CNTL1 = 0x0A,
        CNTL2 = 0x0B,
        ASTC = 0x0C,
        // TS1 (reserved),
        // TS2 (reserved),
        I2CDIS = 0x0F,
        ASAX = 0x10,
        ASAY = 0x11,
        ASAZ = 0x12,
        // RSV (reserved)
    };

    enum ST1Flags : uint8_t
    {
        DRDY = 0b00000001,
        DOR = 0b00000010
    };

    enum ST1States : uint8_t
    {
        DRDY_NORMAL = 0,
        DRDY_READY = 1,
        DOR_NORMAL = 0,
        DOR_OVERRUN = 1
    };

    enum ST2Flags : uint8_t
    {
        BITM = 0b00010000,
        HOFL = 0b00001000
    };

    enum ST2States : uint8_t
    {
        HOFL_NORMAL = 0,
        HOFL_OVERFLOW = 1,
        BITM_14BIT = 0,
        BITM_16BIT = 1
    };

    enum CNTL1FlagsBitOutput : uint8_t
    {
        BIT_14_BIT_OUTPUT = 0b00000000,
        BIT_16_BIT_OUTPUT = 0b00010000,
    };

    enum CNTL1FlagsMode : uint8_t
    {
        MODE_MASK = 0b00001111,
        MODE_POWER_DOWN = 0b00000000,
        MODE_SINGLE_MEASUREMENT = 0b00000001,
        MODE_CONTINUOUS_MEASUREMENT_1_8HZ = 0b00000010,
        MODE_CONTINUOUS_MEASUREMENT_2_100HZ = 0b00000110,
        MODE_EXTERNAL_TRIGGER_MEASUREMENT = 0b00000100,
        MODE_SELF_TEST = 0b00001000,
        MODE_FUSE_ROM_ACCESS = 0b00001111,
    };

    enum CNTL2Flags : uint8_t
    {
        SRST = 0b00000001
    };

    enum ASTCFlags : uint8_t
    {
        SELF = 0b01000000
    };

    struct SelfTestResults
    {
        Values<uint8_t> asa; // ASAX/Y/Z values
        Values<float> sMult; // sensitivity adjustment multiplier values
        Values<int16_t> measurements;
        Values<float> adjusted;
        bool pass;
        SelfTestResults() : asa(3), sMult(3), measurements(3), adjusted(3) {}
    };

    struct Config
    {
        Values<uint8_t> asa;
        Values<float> offset;
        Config() : asa(3), offset(3){};
    };

public:
    AK8963(ISerial *const, const ILogger *const = nullptr);
    AK8963(ISerial *const, const struct Config &, const ILogger *const = nullptr);
    virtual ~AK8963();
    virtual uint8_t whoAmI() const;
    virtual bool selfTest(struct SelfTestResults *out = nullptr);
    virtual int startup(enum CNTL1FlagsMode modeFlag = MODE_CONTINUOUS_MEASUREMENT_1_8HZ);
    virtual void shutdown();
    inline virtual enum CNTL1FlagsMode currentMode() const { return _currentMode; }

    // Must call startup() first
    virtual Values<int16_t> getRawSensorValues();
    virtual Values<float> getSensorValues();

    // Use single-reading mode
    // Do not call startup()
    // TODO: add config data (ASAX/Y/Z) to constructor
    // and move init/reset code out of startup()
    virtual Values<int16_t> getSingleRawSensorValues();

    // Set bit output before calling startup
    virtual void setBitOutput(enum CNTL1FlagsBitOutput bitOutput);

    virtual const enum CNTL1FlagsBitOutput getBitOutput() const { return _bitOutput; }

protected:
    virtual void _changeMode(enum CNTL1FlagsMode mode);
    Values<int16_t> _bytesToInts(const Bytes &bytes) const;
    void _retrieveSensitivityValues();
    void _computeSensitivityMultipliers();

protected:
    static const uint8_t _deviceId = 0x48;
    enum CNTL1FlagsBitOutput _bitOutput = BIT_16_BIT_OUTPUT;
    float _scaleFactor = 4192.0f / 32760;
    enum CNTL1FlagsMode _currentMode = MODE_POWER_DOWN; // init to non-existing mode
    Config _config;
    Values<float> *_sensitivity = nullptr;
};

#endif
