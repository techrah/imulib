#ifndef __D597655C_5F73_4EE2_9375_B1D82750484E__
#define __D597655C_5F73_4EE2_9375_B1D82750484E__

#ifdef ARDUINO

#include <SoftwareSerial.h>
#include "../ILogger.hpp"

namespace serial
{
    class SWLogger : public ILogger
    {
    public:
        SWLogger(uint8_t rxdPin, uint8_t txdPin, unsigned long baudrate)
        {
            _ss = new SoftwareSerial(rxdPin, txdPin);
            _ss->begin(baudrate);
        }
        ~SWLogger()
        {
            delete _ss;
        }
        void print(const char *TAG, const char *val)
        {
            _print(TAG);
            _ss->print(String(val));
        }

        void print(const char *TAG, int val, enum Format format = Format::dec)
        {
            _print(TAG);
            _ss->print(val, _getFormatCode(format));
        }

        void print(const char *TAG, float val, int dp = 2)
        {
            _print(TAG);
            _ss->print(val, dp);
        }

        void println(const char *TAG, const char *val)
        {
            _print(TAG);
            _ss->println(val);
        }

        void println(const char *TAG, int val, enum Format format = Format::dec)
        {
            _print(TAG);
            _ss->println(val, _getFormatCode(format));
        }

        void println(const char *TAG, float val, int dp = 2)
        {
            _print(TAG);
            _ss->println(val, dp);
        }

    private:
        void _print(const char *TAG)
        {
            if (TAG && String(TAG).length() > 0)
            {
                _ss->print("[");
                _ss->print(TAG);
                _ss->print("] ");
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

    private:
        SoftwareSerial *_ss;
    };
};

#endif
#endif
