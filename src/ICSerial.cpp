#include "ICSerial.hpp"
#include "serial/util.hpp"
#include "exceptions.hpp"

ICSerial::ICSerial(ISerial *const serial, Logger *const logger)
    : _serial(serial), _logger(logger)
{
}

ICSerial::~ICSerial()
{
}

void ICSerial::validateDeviceId(uint8_t deviceId, const char *name) const
{
    bool found = false;
    for (int i = 0; i < 3; i++)
    {
        found = whoAmI() == deviceId;
        if (found)
        {
            _logger->log("Found device ID for %s: 0x%02X", name, deviceId);
            break;
        }
        else
        {
            _logger->log("Trying to find device ID for %s: 0x%02X", name, deviceId);
            serial::delay(1000);
        }
    }
    if (!found)
    {
        throw ICSerialDeviceNotFoundException(deviceId);
    }
}
