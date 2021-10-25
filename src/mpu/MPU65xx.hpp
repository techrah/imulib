#ifndef __D9F3235D_FB0E_4C8F_A47F_A5044F884476__
#define __D9F3235D_FB0E_4C8F_A47F_A5044F884476__

#include <stdint.h>
#include "serial/ISerial.hpp"
#include "ICSerial.hpp"
#include "AK8963.hpp"

using namespace serial;

class MPU65xx : public ICSerial
{
public:
    MPU65xx(ISerial *const serial, ILogger *const = nullptr);
    ~MPU65xx();
    bool startup();
    void shutdown();
    uint8_t whoAmI() const;
};

#endif
