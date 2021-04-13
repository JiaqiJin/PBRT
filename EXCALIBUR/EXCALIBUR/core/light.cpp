#include "light.h"
#include "Sampling.h"
#include "scene.h"
#include "Shape.h"

RENDERING_BEGIN

bool VisibilityTester::Unoccluded(const Scene& scene) const {
    return !scene.IntersectP(_p0.spawnRayTo(_p1));
}

Spectrum VisibilityTester::Tr(const Scene& scene, Sampler& sampler) const {
    Ray ray(_p0.spawnRayTo(_p1));
    Spectrum Tr(1.f);
    while (true) {
        SurfaceInteraction si;
        bool hitSurface = scene.Intersect(ray, &si);
        // 如果有交点，且交点处材质不为空，则返回0，透明材质不在此处理
        if (hitSurface && si.primitive->getMaterial() != nullptr)
            return Spectrum(0.f);

        if (ray.medium) {
            Tr *= ray.medium->Tr(ray, sampler);
        }

        if (!hitSurface) {
            break;
        }
        // 交点处的材质为空时会运行到此，重新生成ray
        ray = si.spawnRayTo(_p1);
    }
    return Tr;
}

RENDERING_END