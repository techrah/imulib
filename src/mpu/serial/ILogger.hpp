#ifndef __C1978B14_A092_4B73_8EBE_3CE4991F219A__
#define __C1978B14_A092_4B73_8EBE_3CE4991F219A__

namespace serial
{
    class ILogger
    {
    public:
        enum Format
        {
            dec,
            hex,
            oct,
            // bin,
        };

    public:
        virtual ~ILogger() {}
        virtual void print(const char *TAG, const char *val) = 0;
        virtual void print(const char *TAG, int val, enum Format format = Format::dec) = 0;
        virtual void print(const char *TAG, float val, int dp = 2) = 0;
        virtual void println(const char *TAG, const char *val) = 0;
        virtual void println(const char *TAG, int val, enum Format format = Format::dec) = 0;
        virtual void println(const char *TAG, float val, int dp = 2) = 0;
    };
};

#endif
