#ifndef __D9F3235D_FB0E_4C8F_A47F_A5044F884476__
#define __D9F3235D_FB0E_4C8F_A47F_A5044F884476__

#include <stdint.h>
#include "serial/ISerial.hpp"
#include "serial/Logger.hpp"
#include "ICSerial.hpp"
#include "AK8963.hpp"

using namespace serial;

class MPU65xx : public ICSerial
{
public:
    MPU65xx(ISerial *const serial, Logger *const logger = new Logger());
    ~MPU65xx();
    void startup();
    void shutdown();
    uint8_t whoAmI() const;
    CoordValues<int16_t> getRawSensorValues();
};

#endif
