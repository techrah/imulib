#include "ICSerial.hpp"

ICSerial::ICSerial(ISerial *const serial, Logger *const logger)
    : _serial(serial), _logger(logger)
{
}

ICSerial::~ICSerial()
{
}
