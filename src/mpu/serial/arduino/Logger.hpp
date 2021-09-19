#ifndef __CD2C37DD_8201_4EF1_B965_D41066DEEF66__
#define __CD2C37DD_8201_4EF1_B965_D41066DEEF66__

#ifdef ARDUINO

#include "../ILogger.hpp"

namespace serial
{
    class Logger : public ILogger
    {
    public:
        Logger(unsigned long baud)
        {
            Serial.begin(baud);
        }
        void print(const char *TAG, const char *val)
        {
            _print(TAG);
            Serial.print(String(val));
        }

        void print(const char *TAG, int val, enum Format format = Format::dec)
        {
            _print(TAG);
            Serial.print(val, _getFormatCode(format));
        }

        void print(const char *TAG, float val, int dp = 2)
        {
            _print(TAG);
            Serial.print(val, dp);
        }

        void println(const char *TAG, const char *val)
        {
            _print(TAG);
            Serial.println(val);
        }

        void println(const char *TAG, int val, enum Format format = Format::dec)
        {
            _print(TAG);
            Serial.println(val, _getFormatCode(format));
        }

        void println(const char *TAG, float val, int dp = 2)
        {
            _print(TAG);
            Serial.println(val, dp);
        }

    private:
        void _print(const char *TAG)
        {
            if (TAG && String(TAG).length() > 0)
            {
                Serial.print("[");
                Serial.print(TAG);
                Serial.print("] ");
            }
        }

        int _getFormatCode(enum Format format)
        {
            switch (format)
            {
            case Format::hex:
                return HEX;
            case Format::oct:
                return OCT;
            case Format::dec:
            default:
                return DEC;
            }
        }
    };
};

#endif
#endif
