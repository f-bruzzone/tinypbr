#include <vector>
#include <iostream>

#include "Includes/Renderer.h"
#include "Includes/Sphere.h"
#include "Includes/Light.h"

int main()
{
    int depth, samples, shadowSamples;
    std::cout << "Enter depth: " << std::endl;
    std::cin >> depth;
    std::cout << "Enter samples: " << std::endl;
    std::cin >> samples;
    std::cout << "Enter shadow samples: " << std::endl;
    std::cin >> shadowSamples;

    
    Material nonemissive{Vec3f(0.4, 0.4, 0.3), Vec3f(0.f, 0.f, 0.f)};
    // Material emissive{Vec3f(0.f, 0.f, 0.f), Vec3f(2.f, 1.6f, 1.4f)};
    Material emissive{Vec3f(0.f, 0.f, 0.f), Vec3f{1.0f, 0.85f, 0.6f}};
    emissive.emissive *= 20.f;   // brightness
    
    Plane lightSource(Vec3f{-0.375f, 2.f, -4.25f}, Vec3f{0.75f, 0, 0}, Vec3f{0, 0, 0.4f}, emissive);    // small
    // Plane lightSource(Vec3f{-1., 2.f, -4.25f}, Vec3f{2.f, 0, 0}, Vec3f{0, 0, 0.4f}, emissive);          // large
    
    std::vector<Sphere> spheres{
        Sphere(Vec3f(0.f, -1.f, -4.5f), 0.75f, nonemissive)
        // Sphere(Vec3f(0.f, 2.25f, -4.f), 0.5f, emissive)
        // Sphere(Vec3f(-0.75f, -1.5f, -3.75f), 0.25f, nonemissive)
        // Sphere(Vec3f(-0.75f, -1.45f, -4.75f), 0.65f, nonemissive),
        // Sphere(Vec3f(.3f, -1.6f, -4.45f), 0.4f, emmissive),
        // Sphere(Vec3f(0.75f, -0.75f, -4.75f), 0.35f, nonemissive)
    };
    
    Renderer renderer{712, 712, 65.f, depth, samples, shadowSamples, lightSource};
    renderer.render(spheres);

    return 0;
}