#pragma once

#include "Vec3f.h"
#include "Ray.h"
#include "Material.h"

enum struct Axis
{
    X = 0,
    Y,
    Z
};

class Plane
{
public:
    Plane() {}
    Plane(Axis a, float d1, float d2, float axisPt, Material mat) : axis{a}, dimension1{d1}, dimension2{d2}, axisPoint{axisPt}, material{mat}
    {
        switch (axis)
        {
        case (Axis::X):
            otherAxis1 = Axis::Y;
            otherAxis2 = Axis::Z;
            normal = Vec3f{1, 0, 0};
            break;

        case (Axis::Y):
            otherAxis1 = Axis::X;
            otherAxis2 = Axis::Z;
            normal = Vec3f{0, 1, 0};
            break;

        case (Axis::Z):
            otherAxis1 = Axis::X;
            otherAxis2 = Axis::Y;
            normal = Vec3f{0, 0, 1};
            break;
        }
    }

    Material material;
    Vec3f normal;
    float dimension1;
    float dimension2;
    float axisPoint;
    Axis axis;
    Axis otherAxis1;
    Axis otherAxis2;

    bool intersect(const Ray &ray, Vec3f &hit, float &t0, const float &closest_t) const
    {
        constexpr float EPS = 1e-6;
        if (std::fabs(ray.dir[(int)axis]) < EPS)
            return false;

        t0 = (axisPoint - ray.origin[(int)axis]) / ray.dir[(int)axis];
        if (t0 <= EPS || t0 > closest_t)
            return false;

        auto p = ray.origin + ray.dir * t0;
        if (std::fabs(p[(int)otherAxis1]) > dimension1 ||
            std::fabs(p[(int)otherAxis2]) > dimension2)
            return false;

        hit = p;

        return true;
    }
};