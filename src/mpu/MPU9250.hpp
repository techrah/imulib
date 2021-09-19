#ifndef __E23F575C_250B_4D42_BF57_BD029A41007E__
#define __E23F575C_250B_4D42_BF57_BD029A41007E__

#include "MPU65xx.hpp"
#include "serial/I2C.hpp"
#include "serial/NullLogger.hpp"

class MPU9250 : public MPU65xx
{
public:
    MPU9250(ISerial *const serial,
            I2C *const auxSerial = nullptr,
            ILogger *const logger = new NullLogger());
    MPU9250(const MPU9250 &) = delete;
    MPU9250 &operator=(const MPU9250 &) = delete;
    virtual ~MPU9250();
    void selfTest(struct AK8963::SelfTestResults *magTestResults = nullptr);
    bool startup();
    void shutdown();
    uint8_t whoAmI() const;
    AK8963 *getMag() { return _mag; }

private:
    I2C *_auxSerial;
    AK8963 *_mag;
    static const uint8_t _deviceId = 0x71;
};

#endif