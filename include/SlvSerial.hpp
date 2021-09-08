#ifndef __A59C779E_15A0_4B27_B24D_DD19C551A89C__
#define __A59C779E_15A0_4B27_B24D_DD19C551A89C__

#include "serial/ISerial.hpp"

using namespace serial;

class SlvSerial : public ISerial
{
public:
    static const uint8_t I2C_SLV_ADDR[];
    static const uint8_t I2C_SLV_REG[];
    static const uint8_t I2C_SLV_CTRL[];
    static const uint8_t I2C_SLV_DO[];

    enum I2CSlv : uint8_t
    {
        I2CSlv0,
        I2CSlv1,
        I2CSlv2,
        I2CSlv3,
        I2CSlv4
    };

    enum I2CSlvAddrFlags : uint8_t
    {
        I2C_SLV_WRITE = 0x00,
        I2C_SLV_READ = 0x80
    };

    enum I2CSlvCtrlFlags : uint8_t
    {
        I2C_SLV_GRP = 0x10,
        I2C_SLV_REG_DIS = 0x20,
        I2C_SLV_BYTE_SW = 0x40,
        I2C_SLV_EN = 0x80
    };

    enum ExtSensData : uint8_t
    {
        EXT_SENS_DATA_00 = 0x49,
        EXT_SENS_DATA_01 = 0x4A,
        EXT_SENS_DATA_02 = 0x4B,
        EXT_SENS_DATA_03 = 0x4C,
        EXT_SENS_DATA_04 = 0x4D,
        EXT_SENS_DATA_05 = 0x4E,
        EXT_SENS_DATA_06 = 0x4F,
        EXT_SENS_DATA_07 = 0x50,
        EXT_SENS_DATA_08 = 0x51,
        EXT_SENS_DATA_09 = 0x52,
        EXT_SENS_DATA_10 = 0x53,
        EXT_SENS_DATA_11 = 0x54,
        EXT_SENS_DATA_12 = 0x55,
        EXT_SENS_DATA_13 = 0x56,
        EXT_SENS_DATA_14 = 0x57,
        EXT_SENS_DATA_15 = 0x58,
        EXT_SENS_DATA_16 = 0x59,
        EXT_SENS_DATA_17 = 0x5A,
        EXT_SENS_DATA_18 = 0x5B,
        EXT_SENS_DATA_19 = 0x5C,
        EXT_SENS_DATA_20 = 0x5D,
        EXT_SENS_DATA_21 = 0x5E,
        EXT_SENS_DATA_22 = 0x5F,
        EXT_SENS_DATA_23 = 0x60
    };

public:
    SlvSerial(uint8_t slvno, uint8_t deviceAddr, serial::ISerial *serial);
    void setSlvForRead(enum ExtSensData esd, int reg, uint8_t count);
    void setEsdInitialDelay(unsigned long delayMs) { _esdInitialDelay = delayMs; };
    virtual void writeReg(uint8_t reg, uint8_t data);
    virtual uint8_t readReg(uint8_t reg);
    virtual void readReg(uint8_t reg, uint8_t *out, uint8_t count);
    virtual Bytes readReg(uint8_t reg, uint8_t count);

private:
    uint8_t _deviceAddr;
    uint8_t _slvno;
    ISerial *_serial;

    // Keep track of which register is being used for incoming external sensor data
    // and the actual register number on the external device providing the data.
    enum ExtSensData _esd = EXT_SENS_DATA_00;
    int _esdReg = -1;
    unsigned long _esdInitialDelay = 20;
};

#endif
