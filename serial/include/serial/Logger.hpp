#ifndef __Logger_hpp__
#define __Logger_hpp__

namespace serial
{
    class Logger
    {
    public:
        virtual void log(const char *fmt, ...){};
    };
};

#endif
