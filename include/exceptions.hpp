#ifndef __B2F35E61_5AFD_484A_8646_F724943C2E8A__
#define __B2F35E61_5AFD_484A_8646_F724943C2E8A__

#include "serial/util.hpp"

class ICSerialException : public std::exception
{
public:
    ICSerialException(int errnum_) : std::exception(), errnum(errnum_) {}

    virtual const char *what() const noexcept
    {
        return _msg.c_str();
    }

public:
    int errnum = 0;

protected:
    std::string _msg = "";
};

class ICSerialDeviceNotFoundException : public ICSerialException
{
public:
    ICSerialDeviceNotFoundException(uint8_t deviceId) : ICSerialException(-100)
    {
        _msg = string_format("No device with ID %#02x", deviceId);
    }
};

#endif
