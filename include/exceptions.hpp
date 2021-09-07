#ifndef __B2F35E61_5AFD_484A_8646_F724943C2E8A__
#define __B2F35E61_5AFD_484A_8646_F724943C2E8A__

#include <stdint.h>
#include <memory>
#include <string>
#include <stdexcept>

class ICSerialException : public std::exception
{
public:
    ICSerialException(int errnum_) : std::exception(), errnum(errnum_) {}

    template <typename... Args>
    std::string string_format(const std::string &format, Args... args)
    {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
        if (size_s <= 0)
        {
            throw std::runtime_error("Error during formatting.");
        }
        auto size = static_cast<size_t>(size_s);
        auto buf = std::make_unique<char[]>(size);
        std::snprintf(buf.get(), size, format.c_str(), args...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }

    virtual const char *what() const noexcept
    {
        return _msg.c_str();
    }

public:
    int errnum;

protected:
    std::string _msg;
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
