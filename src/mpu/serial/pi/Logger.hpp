#ifndef __CD2C37DD_8201_4EF1_B965_D41066DEEF66__
#define __CD2C37DD_8201_4EF1_B965_D41066DEEF66__

#ifdef RASPI

#include <stdio.h>
#include <cstring>
#include "../ILogger.hpp"

#define HAS_TAG(TAG) ((TAG) && strlen((TAG)) > 0)

namespace serial
{
    class Logger : public ILogger
    {
    public:
        void print(const char *TAG, const char *val)
        {
            if (HAS_TAG(TAG))
                printf("[%s] %s", TAG, val);
            else
                printf("%s", val);
        }

        void print(const char *TAG, int val, enum Format format = Format::dec)
        {
            const char *fmt;
            switch (format)
            {
            case Format::hex:
                fmt = "%X";
                break;
            case Format::oct:
                fmt = "%o";
                break;
            case Format::dec:
            default:
                fmt = "%d";
            };
            if (HAS_TAG(TAG))
            {
                char buf[100];
                sprintf(buf, fmt, val);
                buf[99] = '\0';
                printf("[%s] %s", TAG, buf);
            }
            else
            {
                printf(fmt, val);
            }
        }

        void print(const char *TAG, float val, int dp = 2)
        {
            if (HAS_TAG(TAG))
                printf("[%s] %0.*f", TAG, dp, val);
            else
                printf("%0.*f", dp, val);
        }

        void println(const char *TAG, const char *val)
        {
            if (HAS_TAG(TAG))
                printf("[%s] %s\r\n", TAG, val);
            else
                printf("%s\r\n", val);
        }

        void println(const char *TAG, int val, enum Format format = Format::dec)
        {
            const char *fmt;
            switch (format)
            {
            case Format::hex:
                fmt = "%X\r\n";
                break;
            case Format::oct:
                fmt = "%o\r\n";
                break;
            case Format::dec:
            default:
                fmt = "%d\r\n";
            };
            if (HAS_TAG(TAG))
            {
                char buf[100];
                sprintf(buf, fmt, val);
                buf[99] = '\0';
                printf("[%s] %s", TAG, buf);
            }
            else
            {
                printf(fmt, val);
            }
        }

        void println(const char *TAG, float val, int dp = 2)
        {
            if (HAS_TAG(TAG))
                printf("[%s] %0.*f\r\n", TAG, dp, val);
            else
                printf("%0.*f\r\n", dp, val);
        }

    private:
        const char *_getFormatCode(enum Format format)
        {
            switch (format)
            {
            case Format::dec:
                return "d";
            case Format::hex:
                return "X";
            case Format::oct:
                return "o";
            default:
                return "";
            }
        }
    };
};

#endif
#endif
