#ifndef __E23F575C_250B_4D42_BF57_BD029A41007E__
#define __E23F575C_250B_4D42_BF57_BD029A41007E__

#include "MPU65xx.hpp"

class MPU9250 : public MPU65xx
{
public:
    MPU9250(ISerial *const serial, Logger *const logger);
    virtual ~MPU9250();
    void startup();
    void shutdown();
    uint8_t whoAmI();
    RawValues getRawSensorValues();

private:
    AK8963 *_mag;
    static const uint8_t _deviceId = 0x71;
};

#endif
