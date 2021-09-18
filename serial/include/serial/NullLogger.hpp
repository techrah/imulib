#ifndef __ABED7C88_E51B_4B8B_B4D6_E31B18295127__
#define __ABED7C88_E51B_4B8B_B4D6_E31B18295127__

#include "ILogger.hpp"

namespace serial
{
    class NullLogger : public ILogger
    {
    public:
        void log(const char *tag, const char *fmt, ...)
        {
        }
    };
};

#endif
