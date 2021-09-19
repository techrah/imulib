#ifndef __ABED7C88_E51B_4B8B_B4D6_E31B18295127__
#define __ABED7C88_E51B_4B8B_B4D6_E31B18295127__

#include "ILogger.hpp"

namespace serial
{
    class NullLogger : public ILogger
    {
    public:
        virtual void print(const char *TAG, const char *val) {}
        virtual void print(const char *TAG, int val, enum Format format = Format::dec) {}
        virtual void print(const char *TAG, float val, int dp) {}
        virtual void println(const char *TAG, const char *val) {}
        virtual void println(const char *TAG, int val, enum Format format = Format::dec) {}
        virtual void println(const char *TAG, float val, int dp) {}
    };
};

#endif
