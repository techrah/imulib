#ifndef __ED1B10CC_AEC4_4BEE_A406_0700745A2EFB__
#define __ED1B10CC_AEC4_4BEE_A406_0700745A2EFB__

#include <stdint.h>
#include "serial/ISerial.hpp"
#include "serial/ILogger.hpp"
#include "serial/NullLogger.hpp"
#include "CoordValues.hpp"

using namespace serial;

class ICSerial
{
public:
    ICSerial(ISerial *const serial, ILogger *const logger = new NullLogger());
    virtual ~ICSerial();
    virtual uint8_t whoAmI() const = 0;
    virtual void validateDeviceId(uint8_t deviceId, const char *name) const;
    virtual void startup() = 0;
    virtual void shutdown() = 0;
    virtual CoordValues<int16_t> getRawSensorValuesSync() = 0;

protected:
    ISerial *const _serial;
    ILogger *const _logger;
};

#endif
