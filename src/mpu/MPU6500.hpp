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
    MPU6500(ISerial *const serial, ILogger *const = nullptr);
    ~MPU6500();
    bool startup();
    void shutdown();
    uint8_t whoAmI() const;
    Values<int16_t> getRawAccelSensorValues();
    Values<float> getAccelSensorValues();

private:
    Values<int16_t> _bytesToInts(const Bytes &data) const;
};

#endif
