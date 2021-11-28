#ifndef __D9F3235D_FB0E_4C8F_A47F_A5044F884476__
#define __D9F3235D_FB0E_4C8F_A47F_A5044F884476__

#include <stdint.h>
#include "serial/ISerial.hpp"
#include "ICSerial.hpp"
#include "AK8963.hpp"

using namespace serial;

class MPU6500 : public ICSerial
{
public:
    enum Registers : uint8_t
    {
        ACCEL_CONFIG = 0X1C,
        ACCEL_CONFIG_2 = 0x1D,
        ACCEL_XOUT_H = 0x3B,
        PWR_MGMT_1 = 0x6B,
        WHO_AM_I = 0x75,
    };

    enum AFS_SEL : uint8_t
    {
        AFS_SEL_2G = 0,
        AFS_SEL_4G = 1,
        AFS_SEL_8G = 2,
        AFS_SEL_16G = 3,
    };

public:
    MPU6500(ISerial *const serial, ILogger *const = nullptr);
    ~MPU6500();
    void setAccelSensitivity(enum AFS_SEL);
    bool startup();
    void shutdown();
    uint8_t whoAmI() const;
    int setAccelDLPF(uint8_t a_dlpf_cfg);
    Values<int16_t> getRawAccelSensorValues();
    Values<float> getAccelSensorValues();

protected:
    enum AFS_SEL _afsSel;
    float _aScale; // accelerometer scale

private:
    Values<int16_t> _bytesToInts(const Bytes &data) const;
};

#endif
