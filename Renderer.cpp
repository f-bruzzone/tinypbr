#include <fstream>
#include <vector>
#include <numbers>
#include <iostream>
#include <filesystem>
#include <thread>

#include "Includes/Renderer.h"
#include "Includes/Vec3f.h"

Renderer::Renderer(int width, int height, float fov, int depth, int samples, Plane& light)
    : _width{width}, _height{height}, _fov{fov}, _maxDepth{depth}, _samples{samples}
{
    _light = light;
    _planes.push_back(light);
    createWalls();
}

void Renderer::render(const std::vector<Sphere> &spheres)
{
    std::vector<Vec3f> frameBuffer(_width * _height);
    Vec3f origin{0, 0, 0};
    float aspect = _width / (float)_height;
    float scale = std::tan(M_PI / (float)180 * 0.5f * _fov);
    unsigned int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads(numThreads);

    for (size_t t = 0; t < numThreads; t++)
    {
        threads[t] = std::thread([&, t]()
                                 {
                                    try{
            for (size_t i = 0; i < _height; i++)
            {
                for (size_t j = 0; j < _width; j++)
                {
                    auto x = ((2 * (j + 0.5f) / (float)_width) - 1) * scale * aspect;
                    auto y = -((2 * (i + 0.5f) / (float)_height) - 1) * scale;
                    Ray ray(Vec3f(x, y, -1.f).normalize(), origin);
                    frameBuffer[i * _width + j] = castRay(ray, spheres, 0);
                }
            } } 
            catch(const std::exception& ex){
                std::cerr << "Thread " << t << " threw: " << ex.what() << std::endl;
            }
            catch(...){
                std::cerr << "Thread " << t << " threw an unknown exception" << std::endl;
            } });
    }

    for (auto &t : threads)
        t.join();

    std::ofstream ofs;
    std::filesystem::path outputPath =
        std::filesystem::current_path().parent_path() / "Image" / "out.ppm";

    std::filesystem::create_directories(outputPath.parent_path());

    ofs.open(outputPath);

    if (!ofs || !ofs.is_open())
        std::cerr << "Failed to open file" << std::endl;

    ofs << "P6\n"
        << _width << " " << _height << "\n255\n";

    float gamma = 2.2f;
    for (size_t i = 0; i < _width * _height; i++)
    {
        Vec3f &c = frameBuffer[i];
        c[0] = std::min(1.f, c[0]);
        c[1] = std::min(1.f, c[1]);
        c[2] = std::min(1.f, c[2]);
        // Reinhard
        // c[0] = c[0] / (c[0] + 1);
        // c[1] = c[1] / (c[1] + 1);
        // c[2] = c[2] / (c[2] + 1);

        // gamma correction:
        // c[0] = std::pow(c[0], 1/gamma);
        // c[1] = std::pow(c[1], 1/gamma);
        // c[2] = std::pow(c[2], 1/gamma);

        for (size_t j = 0; j < 3; j++)
        {
            ofs << (unsigned char)(255 * frameBuffer[i][j] + 0.5f);
        }
    }

    ofs.close();
}

Vec3f Renderer::castRay(const Ray &ray, const std::vector<Sphere> &spheres, int depth)
{
    Material material;
    Vec3f hit, N;

    if (depth >= _maxDepth || !sceneIntersect(ray, spheres, hit, N, material))
        return Vec3f(0.f, 0.f, 0.f); // background color

    if (ray.dir.dot(N) > 0)
        N = N * -1;

    if(material.emissive[0] > 0 || material.emissive[1] > 0 || material.emissive[2] > 0)
        return material.emissive;
        
    Vec3f indirectLo;
    // handle reflections
    //  auto reflectDir = ray.dir.reflect(N).normalize();
    //  auto reflectOrig = reflectDir.dot(N) > 0 ? hit + N * 1e-3 : hit - N * 1e-3;
    //  auto reflected = Ray(reflectDir, reflectOrig);
    //  indirectLo += castRay(reflected, spheres, depth + 1) * material.albedo;

    for (size_t i = 0; i < _samples; i++)
    {
        auto randomRay = Ray::genRayFromIntersection(N, hit + N * 1e-3);
        auto cos = std::max(0.f, randomRay.dir.dot(N));
        indirectLo += castRay(randomRay, spheres, depth + 1) * material.albedo;
    }
    indirectLo /= _samples;

    auto Li = _light.getVecToPlaneFromHit(hit);
    auto wi = Li.normalize();
    auto cosTheta = std::max(0.f, wi.dot(N));
    auto lightDist2 = Li.dot(Li);
    
    Vec3f directLo;
    // handle shadows
    auto shadowOrigin = cosTheta <= 0 ? hit - N * 1e-3 : hit + N * 1e-3;
    Vec3f shadowHit, shadowN;
    Material tmpMat;
    bool inShadow = sceneIntersect(Ray(wi, shadowOrigin), spheres, shadowHit, shadowN, tmpMat)
                    && lightDist2 - 1e-3 > (shadowHit - shadowOrigin).dot(shadowHit - shadowOrigin)
                    && !(tmpMat.emissive[0] > 0 || tmpMat.emissive[1] > 0 || tmpMat.emissive[2] > 0);

    if (!inShadow)
        directLo += (material.albedo / std::numbers::pi) * cosTheta * _light.material.emissive;
        // directLo += material.albedo * cosTheta; // direct lighting only

    return directLo + indirectLo;
}

bool Renderer::sceneIntersect(const Ray &ray, const std::vector<Sphere> &spheres, Vec3f &hit, Vec3f &N, Material &material)
{
    float closest_t = std::numeric_limits<float>::max();

    float t0 = 0.f;
    for (const auto &sphere : spheres)
    {
        if (!sphere.intersect(ray, t0) || t0 >= closest_t)
            continue;

        closest_t = t0;
        hit = ray.origin + ray.dir * t0;
        N = (hit - sphere.center).normalize();
        material = sphere.material;
    }

    for (const auto &plane : _planes)
    {
        if (!plane.intersect(ray, hit, t0, closest_t) || t0 >= closest_t)
            continue;

        closest_t = t0;
        N = plane.N;

        material = plane.material;
    }

    return closest_t < std::numeric_limits<float>::max();
}

void Renderer::createWalls()
{
    auto cream = Material{Vec3f(0.8f, 0.8f, 0.8f), Vec3f(0, 0, 0)};
    auto red = Material{Vec3f(0.7f, 0.2f, 0.2f), Vec3f(0, 0, 0)};
    auto green = Material{Vec3f(0.f, 0.7f, 0.2f), Vec3f(0, 0, 0)};

    _planes.emplace_back(Vec3f{-2.f, 2.f, -6.f}, Vec3f{4, 0, 0}, Vec3f{0, 0, 7}, cream);  // ceiling
    _planes.emplace_back(Vec3f{2.f, 2.f, -6.f}, Vec3f{-6, 0, 0}, Vec3f{0, -6, 0}, cream); // back wall
    _planes.emplace_back(Vec3f{2.f, -2.f, 0.f}, Vec3f{0, 0, -7}, Vec3f{-4, 0, 0}, cream); // floor
    _planes.emplace_back(Vec3f{2.f, -2.f, 0.f}, Vec3f{0, 4, 0}, Vec3f{0, 0, -6}, green);  // right wall
    _planes.emplace_back(Vec3f{-2.f, -2.f, -6.f}, Vec3f{0, 4, 0}, Vec3f{0, 0, 6}, red);   // left wall
}