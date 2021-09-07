#ifndef __C1978B14_A092_4B73_8EBE_3CE4991F219A__
#define __C1978B14_A092_4B73_8EBE_3CE4991F219A__

namespace serial
{
    class Logger
    {
    public:
        virtual void log(const char *fmt, ...){};
    };
};

#endif
