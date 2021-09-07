#ifndef __A65E8222_D7CE_4904_BDB9_41B3E9028F88__
#define __A65E8222_D7CE_4904_BDB9_41B3E9028F88__

#include "ICSerial.hpp"

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

public:
    AK8963(ISerial *const serial, Logger *const logger = new Logger());
    virtual ~AK8963();
    virtual uint8_t whoAmI();
    virtual void startup();
    virtual void shutdown();
    virtual RawValues getRawSensorValues();

private:
    uint8_t _deviceId = 0x48;
};

#endif
