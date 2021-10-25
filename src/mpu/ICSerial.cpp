#include "ICSerial.hpp"
#include "serial/util.hpp"

static const char *TAG = "ICSerial";

ICSerial::ICSerial(ISerial *const serial, ILogger *const logger)
    : _serial(serial), _logger(logger)
{
    if (!logger)
    {
        _logger = new NullLogger();
    }
}

ICSerial::~ICSerial()
{
    delete _logger;
}

bool ICSerial::validateDeviceId(uint8_t deviceId, const char *name) const
{
    bool found = false;
    for (unsigned i = 0; i < 3; i++)
    {
        uint8_t actualDeviceId = whoAmI();
        found = actualDeviceId == deviceId;
        if (found)
        {
            _logger->print(TAG, "Found device ID for ");
            _logger->print(NULL, name);
            _logger->print(NULL, ": 0x");
            _logger->println(NULL, deviceId, ILogger::Format::hex);
            break;
        }
        else
        {
            _logger->print(TAG, "Found device ID: ");
            _logger->println(NULL, actualDeviceId, ILogger::Format::hex);
            _logger->print(TAG, "Trying to find device ID for ");
            _logger->print(NULL, name);
            _logger->print(NULL, ": 0x");
            _logger->println(NULL, deviceId, ILogger::Format::hex);
            serial::delay(1000);
        }
    }
    return found;
}
