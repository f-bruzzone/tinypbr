#pragma once

#include <array>

#include "Vec3f.h"

struct Matf3
{
    Mat3f(Vec3f &a, Vec3f &b, Vec3f &c) : _values[0][0] = a[0], _values[1][0] = a[1], _values[2][0] = a[2],
                                          _values[0][1] = b[0], _values[1][1] = b[1], _values[2][1] = b[2],
                                          _values[0][2] = c[0], _values[1][2] = c[1], _values[2][2] = c[2] {}

    std::array<std::array<float, 3>, 3> _values;

    inline Vec3f operator*(const Vec3f &v)
    {
        Vec3f out;
        out[0] = _values[0][0] * v[0] + _values[0][1] * v[1] + _values[0][2] * v[2];
        out[1] = _values[1][0] * v[0] + _values[1][1] * v[1] + _values[1][2] * v[2];
        out[2] = _values[2][0] * v[0] + _values[2][1] * v[1] + _values[2][2] * v[2];
        return out;
    }
}