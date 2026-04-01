#pragma once

#include "Ray.h"
#include "Sphere.h"
#include "Light.h"
#include "Plane.h"

class Renderer
{
public:
    Renderer(int width, int height, float fov, int depth, int samples);

    void render(const std::vector<Sphere> &sphere, const Light &light);

private:
    std::vector<Plane> _planes;
    const float _fov;
    const int _width;
    const int _height;
    const int _maxDepth;
    const int _samples;

    Vec3f castRay(const Ray &ray, const std::vector<Sphere> &spheres, const Light &light, int depth);
    bool sceneIntersect(const Ray &ray, const std::vector<Sphere> &spheres, Vec3f &hit, Vec3f &N, Material &material);
};