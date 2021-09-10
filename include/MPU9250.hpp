#ifndef __E23F575C_250B_4D42_BF57_BD029A41007E__
#define __E23F575C_250B_4D42_BF57_BD029A41007E__

#include "MPU65xx.hpp"
#include "serial/I2C.hpp"

class MPU9250 : public MPU65xx
{
public:
    MPU9250(ISerial *const serial,
            I2C *const auxSerial = nullptr,
            Logger *const logger = new Logger());
    virtual ~MPU9250();
    void startup();
    void shutdown();
    uint8_t whoAmI() const;
    RawValues getRawSensorValues();

private:
    I2C *_auxSerial;
    AK8963 *_mag;
    static const uint8_t _deviceId = 0x71;
};

#endif
