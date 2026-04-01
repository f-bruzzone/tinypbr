#include <vector>
#include <iostream>

#include "Includes/Renderer.h"
#include "Includes/Sphere.h"
#include "Includes/Light.h"

int main()
{
    int depth;
    int samples;
    std::cout << "Enter depth: " << std::endl;
    std::cin >> depth;
    std::cout << "Enter samples: " << std::endl;
    std::cin >> samples;

    Renderer renderer{712, 712, 65.f, depth, samples};

    Material emissive{Vec3f(1, 1, 1), Vec3f(1, 1, 1)};
    Material nonemissive{Vec3f(0.4, 0.4, 0.3), Vec3f(0.f, 0.f, 0.f)};

    Light light{Vec3f(0, 1.49f, -4.f), 5.f};

    std::vector<Sphere> spheres{
        Sphere(Vec3f(0.f, -1.f, -4.5f), 0.75f, nonemissive),
        Sphere(Vec3f(0.f, 2.25f, -4.f), 0.5f, emissive)
        // Sphere(Vec3f(-0.75f, -1.5f, -3.75f), 0.25f, nonemissive)
        // Sphere(Vec3f(-0.75f, -1.45f, -4.75f), 0.65f, nonemissive),
        // Sphere(Vec3f(.3f, -1.6f, -4.45f), 0.4f, emmissive),
        // Sphere(Vec3f(0.75f, -0.75f, -4.75f), 0.35f, nonemissive)
    };

    renderer.render(spheres, light);

    return 0;
}