#ifndef __B61D9AD9_74B3_48F7_A109_D6957EBF5E6A__
#define __B61D9AD9_74B3_48F7_A109_D6957EBF5E6A__

#include "stdexcept"

namespace serial
{
    class SerialException : public std::exception
    {
    public:
        SerialException(int errnum_, const std::string &msg)
            : std::exception(), errnum(errnum_), _msg(msg) {}

        virtual const char *what() const noexcept
        {
            return _msg.c_str();
        }

    public:
        int errnum = 0;

    protected:
        std::string _msg = "";
    };

    class IndexException
    {
    };
}

#endif
