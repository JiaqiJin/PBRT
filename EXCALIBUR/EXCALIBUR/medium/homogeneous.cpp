#include "homogeneous.h"
#include "../core/Interaction.h"
#include "../core/sampler.h"

RENDERING_BEGIN

// Tr(p → p′) = e^(−σt * l)  volpt.hpp 中 9.1式
Spectrum HomogeneousMedium::Tr(const Ray& ray, Sampler& sampler) const {
    TRY_PROFILE(Prof::MediumTr)
        return Exp(-_sigma_t * std::min(ray.tMax * ray.dir.length(), MaxFloat));
}

// 采样参与介质，生成MediumInteraction对象
Spectrum HomogeneousMedium::sample(const Ray& ray, Sampler& sampler, MemoryArena& arena, MediumInteraction* mi) const {
    TRY_PROFILE(Prof::MediumSample)

        int channel = std::min((int)(sampler.Get1D() * Spectrum::nSamples),
            Spectrum::nSamples - 1);
    //         - ln(1 - ξ)
    // dist = -------------
    //             σt
    Float dist = -std::log(1 - sampler.Get1D()) / _sigma_t[channel];
    Float t = std::min(dist / ray.dir.length(), ray.tMax);
    // 是否采样到介质中
    bool sampledMedium = t < ray.tMax;
    if (sampledMedium) {
        *mi = MediumInteraction(ray.at(t), -ray.dir, ray.time, this,
            ARENA_ALLOC(arena, HenyeyGreenstein)(_g));
    }

    // Tr(p → p′) = e^(−σt * l)
    Spectrum Tr = Exp(-_sigma_t * std::min(t, MaxFloat) * ray.dir.length());
    Spectrum density = sampledMedium ? (_sigma_t * Tr) : Tr;
    Float pdf = 0;
    for (int i = 0; i < Spectrum::nSamples; ++i) {
        pdf += density[i];
    }
    pdf *= 1 / (Float)Spectrum::nSamples;
    if (pdf == 0) {
        DCHECK(Tr.IsBlack());
        pdf = 1;
    }
    return sampledMedium ? (Tr * _sigma_s / pdf) : (Tr / pdf);
}
       

RENDERING_END