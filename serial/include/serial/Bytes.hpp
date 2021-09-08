#ifndef __DACFDA9A_019B_4E89_89C4_7C706C5D73BB__
#define __DACFDA9A_019B_4E89_89C4_7C706C5D73BB__

#include <stdint.h>
#include <stdio.h>
#include "exceptions.hpp"

namespace serial
{

    class Bytes
    {
    private:
        uint8_t _count;
        uint8_t *_data = nullptr;

    public:
        Bytes(uint8_t count) : _count(count) { _data = new uint8_t[count](); }
        Bytes(Bytes &&rhs) : _data(rhs._data) { rhs._data = nullptr; }
        virtual ~Bytes() { delete _data; }
        uint8_t operator[](uint8_t ix) const
        {
            _validateIndex(ix);
            return _data[ix];
        }
        uint8_t &operator[](uint8_t ix)
        {
            _validateIndex(ix);
            return _data[ix];
        }
        // direct access (unsafe)
        uint8_t *dataBuf() { return _data; }

    private:
        inline void _validateIndex(uint8_t ix) const
        {
            if (ix - 1 > _count)
            {
                throw new IndexException();
            }
        }
    };
}
#endif
