#ifndef __A59C779E_15A0_4B27_B24D_DD19C551A89C__
#define __A59C779E_15A0_4B27_B24D_DD19C551A89C__

#include "serial/ISerial.hpp"
#include "serial/ILogger.hpp"
#include "serial/NullLogger.hpp"

using namespace serial;

struct SlvConnection;

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

    static const uint8_t I2C_SLV4_ADDR = 0x31;
    static const uint8_t I2C_SLV4_REG = 0x32;
    static const uint8_t I2C_SLV4_DO = 0x33;
    static const uint8_t I2C_SLV4_CTRL = 0x34;
    static const uint8_t I2C_SLV4_DI = 0x35;

public:
    SlvSerial(uint8_t deviceAddr, serial::ISerial *serial, ILogger *logger = new NullLogger());
    virtual ~SlvSerial();
    void setEsdInitialDelay(unsigned long delayMs) { _esdInitialDelay = delayMs; };
    virtual void writeReg(uint8_t reg, uint8_t data);
    virtual uint8_t readReg(uint8_t reg);
    virtual Bytes readReg(uint8_t reg, uint8_t count);

private:
    void _setSlvForRead(const SlvConnection &conn);
    SlvConnection _addConnection(uint8_t reg, uint8_t count);
    SlvConnection _getConnection(uint8_t reg, uint8_t count);
    void _clearAllConnections();

private:
    uint8_t _deviceAddr;
    ISerial *_serial;
    ILogger *_logger;
    SlvConnection *_slvConnections;
    uint8_t _numConn = 0;
    unsigned long _esdInitialDelay = 1;
};

struct SlvConnection
{
    uint8_t slvno;
    uint8_t reg;
    uint8_t count;
    SlvSerial::ExtSensData esd;
};

#endif
