#ifndef __E23F575C_250B_4D42_BF57_BD029A41007E__
#define __E23F575C_250B_4D42_BF57_BD029A41007E__

#include "MPU6500.hpp"
#include "serial/I2C.hpp"

#include "../linalg/BasicLinearAlgebra.h"

class MPU9250 : public MPU6500
{
public:
    MPU9250(ISerial *const serial,
            I2C *const auxSerial = nullptr,
            ILogger *const = nullptr);
    MPU9250(const MPU9250 &) = delete;
    MPU9250 &operator=(const MPU9250 &) = delete;
    virtual ~MPU9250();
    void setMagConfig(const AK8963::Config &);
    void selfTest(struct AK8963::SelfTestResults *magTestResults = nullptr);
    bool startup();
    void shutdown();
    uint8_t whoAmI() const;
    AK8963 *getMag() { return _mag; }
    Vector<3> getUprightMagValues();
    Matrix<3, 3> getZCorrectionTransform();

private:
    I2C *_auxSerial;
    AK8963 *_mag;
    static const uint8_t _deviceId = 0x71;
};

#endif
