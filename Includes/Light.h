#pragma once

#include "Vec3f.h"

struct Light
{
    Light(Vec3f &&p, float i) : position{p}, intensity{i} {}
    Vec3f position{0, 0, 0};
    float intensity;
};