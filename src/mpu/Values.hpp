#ifndef __FDF5CCD3_86F8_4770_A796_F158DAE97D09__
#define __FDF5CCD3_86F8_4770_A796_F158DAE97D09__

#include <stdint.h>
#include <string.h>

template <typename T>
class Values
{
public:
    inline T operator[](uint8_t ix) const
    {
        assert(ix < _size);
        return _data[ix];
    }

    inline T &operator[](uint8_t ix)
    {
        assert(ix < _size);
        return _data[ix];
    }

    Values<T>(uint8_t size) : _size(size), _data(new T[size]) {}

    template <typename U>
    Values<T>(uint8_t size, const U values[]);

    // copy constructor
    Values<T>(const Values<T> &rhs)
    {
        _size = rhs._size;
        _data = new T[_size];
        memcpy(_data, rhs._data, bytes());
    }

    // move constructor
    Values<T>(Values<T> &&rhs)
    {
        _size = rhs._size;
        _data = rhs._data;
        rhs._data = nullptr;
    }

    ~Values()
    {
        delete _data;
    }

    // copy assign
    Values<T> &operator=(const Values<T> &rhs)
    {
        if (this != &rhs)
        {
            delete _data;
            _size = rhs._size;
            _data = new T[_size];
            memcpy(_data, rhs._data, bytes());
        }
        return *this;
    }

    // move assign
    Values<T> &operator=(Values<T> &&rhs)
    {
        if (this != &rhs && _size == rhs._size)
        {
            _data = rhs._data;
            rhs._data = nullptr;
        }
        return *this;
    }

    // ex: v = (const int[]){175, 175, 164};
    // Unsafe! array on RHS should have same number of elements
    template <typename U>
    Values<T> &operator=(const U values[])
    {
        for (int i = 0; i < _size; i++)
        {
            _data[i] = static_cast<T>(values[i]);
        }
        return *this;
    }

    // conversion operator for casting from type T to U
    template <typename U>
    operator Values<U>()
    {
        Values<U> res(_size);
        for (unsigned i = 0; i < _size; i++)
        {
            res[i] = static_cast<U>(_data[i]);
        }
        return res;
    }

    // {x, y, z} * n = {nx, ny, nz}
    template <typename U>
    Values<T> operator*(U scalar) const
    {
        Values<T> res(_size);
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
    Values<T> operator*(const Values<U> &multipliers)
    {
        Values<T> res(_size);
        for (unsigned i = 0; i < _size; i++)
        {
            res[i] = _data[i] * static_cast<T>(multipliers[i]);
        }
        return res;
    };

    // {x, y, z} *= {a, b, c} => {ax, by, cz}
    template <typename U>
    void operator*=(const Values<U> &multipliers)
    {
        for (unsigned i = 0; i < _size; i++)
        {
            _data[i] *= static_cast<T>(multipliers[i]);
        }
    };

    template <typename Functor>
    Values<T> &apply(Functor &fn)
    {
        for (unsigned i = 0; i < _size; i++)
        {
            float res = fn(_data[i], i);
            _data[i] = res;
        }
        return *this;
    }

    inline T *data() const
    {
        return _data;
    }

    inline uint8_t bytes() const
    {
        return _size * sizeof(T);
    }

protected:
    uint8_t _size;
    T *_data;
};

// ex: Values<int>(3, (const int[]){1, 2, 3})
// Example may not compile with g++
template <typename T>
template <typename U>
Values<T>::Values(uint8_t size, const U values[]) : _size(size)
{
    *this = values;
}

#endif
