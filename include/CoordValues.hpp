#ifndef __FDF5CCD3_86F8_4770_A796_F158DAE97D09__
#define __FDF5CCD3_86F8_4770_A796_F158DAE97D09__

#include <algorithm>
#include <stdexcept>
#include <stdint.h>
#include <vector>

using namespace std;

template <typename T>
class CoordValues
{
public:
    T operator[](uint8_t ix) const
    {
        _validateIx(ix);
        return _data[ix];
    }
    T &operator[](uint8_t ix)
    {
        _validateIx(ix);
        return _data[ix];
    }

    CoordValues<T>(uint8_t size = 3) : _data(vector<T>(size, {})) {}
    CoordValues<T>(const vector<T> &values) : _data(values) {}

    // copy constructor
    CoordValues<T>(const CoordValues<T> &rhs) : _data(rhs._data) {}

    // move constructor
    CoordValues<T>(const CoordValues<T> &&rhs) : _data(move(rhs._data)) {}

    // copy assign
    const CoordValues<T> &operator=(const CoordValues<T> &rhs)
    {
        _data = rhs._data;
        return *this;
    }

    // move assign
    const CoordValues<T> &operator=(const CoordValues<T> &&rhs)
    {
        _data = move(rhs._data);
        return *this;
    }

    // conversion operator for casting from one type to another
    template <typename U>
    operator CoordValues<U>()
    {
        auto cast = [](auto v)
        { return static_cast<U>(v); };

        vector<U> res(_data.size());
        transform(_data.begin(), _data.end(), res.begin(), cast);
        return CoordValues<U>(res);
    }

    // {x, y, z} * n = {nx, ny, nz}
    template <typename V>
    CoordValues<T> operator*(V scalar) const
    {
        auto multiply = [scalar](T val)
        { return val * scalar; };

        vector<T> res(_data.size());
        transform(_data.begin(), _data.end(), res.begin(), multiply);
        return CoordValues<T>(res);
    };

    // {x, y, z} *= n => {nx, ny, nz}
    template <typename V>
    void operator*=(V scalar)
    {
        auto multiply = [scalar](T val)
        { return val * scalar; };

        transform(_data.begin(), _data.end(), _data.begin(), multiply);
    };

    // {x, y, z} * {a, b, c} = {ax, by, cz}
    template <typename V>
    CoordValues<T> operator*(vector<V> multipliers)
    {
        auto multiply = [](const auto &val, const auto &multiplier)
        { return val * multiplier; };

        vector<T> res(_data.size());
        transform(_data.begin(), _data.end(), multipliers.begin(), res.begin(), multiply);
        return res;
    };

    // {x, y, z} *= {a, b, c} => {ax, by, cz}
    template <typename V>
    void operator*=(vector<V> multipliers)
    {
        auto multiply = [](const auto &val, const auto &multiplier)
        { return val * multiplier; };

        transform(_data.begin(), _data.end(), multipliers.begin(), _data.begin(), multiply);
    };

    const vector<T> &data()
    {
        return _data;
    }

protected:
    vector<T> _data;

    void _validateIx(uint8_t ix) const
    {
        if (ix >= _data.size())
        {
            throw std::out_of_range("std::vector index out of bounds");
        }
    }
};

#endif
