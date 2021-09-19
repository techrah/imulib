#ifndef __FDF5CCD3_86F8_4770_A796_F158DAE97D09__
#define __FDF5CCD3_86F8_4770_A796_F158DAE97D09__

#include <stdint.h>
#include <string.h>

template <typename T>
class CoordValues
{
public:
    T operator[](uint8_t ix) const
    {
        if (_validateIx(ix))
            return _data[ix];
    }

    T &operator[](uint8_t ix)
    {
        if (_validateIx(ix))
            return _data[ix];
    }

    CoordValues<T>(uint8_t size) : _data(new T[size]), _size(size) {}

    // copy constructor
    CoordValues<T>(const CoordValues<T> &rhs)
    {
        _data = new T[rhs._size];
        _size = rhs._size;
        memcpy(_data, rhs._data, bytes());
    }

    // move constructor
    CoordValues<T>(CoordValues<T> &&rhs)
    {
        _data = rhs._data;
        _size = rhs._size;
        rhs._data = nullptr;
    }

    ~CoordValues()
    {
        delete _data;
    }

    // copy assign
    CoordValues<T> &operator=(const CoordValues<T> &rhs)
    {
        if (this != &rhs)
        {
            delete _data;
            _data = new T[rhs._size];
            _size = rhs._size;
            memcpy(_data, rhs._data, bytes());
        }
        return *this;
    }

    // move assign
    CoordValues<T> &operator=(CoordValues<T> &&rhs)
    {
        if (this != &rhs)
        {
            _data = rhs._data;
            _size = rhs._size;
            rhs._data = nullptr;
        }
        return *this;
    }

    // conversion operator for casting from type T to U
    template <typename U>
    operator CoordValues<U>()
    {
        CoordValues<U> res(_size);
        for (unsigned i = 0; i < _size; i++)
        {
            res[i] = static_cast<U>(_data[i]);
        }
        return res;
    }

    // {x, y, z} * n = {nx, ny, nz}
    template <typename U>
    CoordValues<T> operator*(U scalar) const
    {
        CoordValues<T> res(_size);
        for (unsigned i = 0; i < _size; i++)
        {
            res[i] = _data[i] * static_cast<T>(scalar);
        }
        return res;
    };

    // {x, y, z} *= n => {nx, ny, nz}
    template <typename U>
    void operator*=(U scalar)
    {
        for (unsigned i = 0; i < _size; i++)
        {
            _data[i] *= static_cast<T>(scalar);
        }
    };

    // {x, y, z} * {a, b, c} = {ax, by, cz}
    template <typename U>
    CoordValues<T> operator*(const CoordValues<U> &multipliers)
    {
        CoordValues<T> res(_size);
        for (unsigned i = 0; i < _size; i++)
        {
            res[i] = _data[i] * static_cast<T>(multipliers[i]);
        }
        return res;
    };

    // {x, y, z} *= {a, b, c} => {ax, by, cz}
    template <typename U>
    void operator*=(const CoordValues<U> &multipliers)
    {
        for (unsigned i = 0; i < _size; i++)
        {
            _data[i] *= static_cast<T>(multipliers[i]);
        }
    };

    T *data() const
    {
        return _data;
    }

    inline uint8_t bytes() const
    {
        return sizeof(*_data) * sizeof(T);
    }

protected:
    T *_data;
    uint8_t _size;

    bool _validateIx(uint8_t ix) const
    {
        return ix < _size;
    }
};

#endif
