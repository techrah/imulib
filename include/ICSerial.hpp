#ifndef __ED1B10CC_AEC4_4BEE_A406_0700745A2EFB__
#define __ED1B10CC_AEC4_4BEE_A406_0700745A2EFB__

#include <stdint.h>
#include "serial/ISerial.hpp"
#include "serial/Logger.hpp"
#include "RawValues.hpp"

using namespace serial;

class ICSerial
{
public:
    ICSerial(ISerial *const serial, Logger *const logger = new Logger());
    virtual ~ICSerial();
    virtual uint8_t whoAmI() const = 0;
    virtual void validateDeviceId(uint8_t deviceId, const char *name) const;
    virtual void startup() = 0;
    virtual void shutdown() = 0;
    virtual RawValues getRawSensorValues() = 0;

protected:
    ISerial *const _serial;
    Logger *const _logger;
};

#endif
