#ifndef __DACFDA9A_019B_4E89_89C4_7C706C5D73BB__
#define __DACFDA9A_019B_4E89_89C4_7C706C5D73BB__

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

namespace serial
{
    class Bytes
    {
    private:
        uint8_t _count;
        uint8_t *_data = nullptr;
        uint8_t _offset;
        uint8_t _maxIx;

    public:
        Bytes(uint8_t count, int8_t offset = 0)
            : _count(count), _offset(offset), _maxIx(count + offset - 1)
        {
            _data = new uint8_t[count + offset]();
        }
        Bytes(Bytes &&rhs) : _data(rhs._data) { rhs._data = nullptr; }
        virtual ~Bytes() { delete _data; }
        uint8_t operator[](uint8_t ix) const
        {
            assert(ix <= _maxIx);
            uint8_t newIx = ix + _offset;
            return _data[newIx];
        }
        uint8_t &operator[](uint8_t ix)
        {
            assert(ix <= _maxIx);
            uint8_t newIx = ix + _offset;
            return _data[newIx];
        }
        // direct access (unsafe)
        uint8_t *dataBuf() { return _data; }

    private:
        inline bool _validateIndex(uint8_t ix) const
        {
            return ix <= _maxIx;
        }
    };
}
#endif
