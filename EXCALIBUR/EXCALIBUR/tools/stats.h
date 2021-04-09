#pragma once

#include "../core/Header.h"
#include <map>
#include <chrono>
#include <string>
#include <functional>
#include <mutex>

RENDERING_BEGIN

class StatsAccumulator;
class StatRegisterer {
public:
    // StatRegisterer Public Methods
    StatRegisterer(std::function<void(StatsAccumulator&)> func) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (!funcs)
            funcs = new std::vector<std::function<void(StatsAccumulator&)>>;
        funcs->push_back(func);
    }
    static void CallCallbacks(StatsAccumulator& accum);

private:
    // StatRegisterer Private Data
    static std::vector<std::function<void(StatsAccumulator&)>>* funcs;
};

void PrintStats(FILE* dest);
void ClearStats();
void ReportThreadStats();

class StatsAccumulator {
public:
    // StatsAccumulator Public Methods
    void ReportCounter(const std::string& name, int64_t val) {
        counters[name] += val;
    }
    void ReportMemoryCounter(const std::string& name, int64_t val) {
        memoryCounters[name] += val;
    }
    void ReportIntDistribution(const std::string& name, int64_t sum,
        int64_t count, int64_t min, int64_t max) {
        intDistributionSums[name] += sum;
        intDistributionCounts[name] += count;
        if (intDistributionMins.find(name) == intDistributionMins.end())
            intDistributionMins[name] = min;
        else
            intDistributionMins[name] =
            std::min(intDistributionMins[name], min);
        if (intDistributionMaxs.find(name) == intDistributionMaxs.end())
            intDistributionMaxs[name] = max;
        else
            intDistributionMaxs[name] =
            std::max(intDistributionMaxs[name], max);
    }
    void ReportFloatDistribution(const std::string& name, double sum,
        int64_t count, double min, double max) {
        floatDistributionSums[name] += sum;
        floatDistributionCounts[name] += count;
        if (floatDistributionMins.find(name) == floatDistributionMins.end())
            floatDistributionMins[name] = min;
        else
            floatDistributionMins[name] =
            std::min(floatDistributionMins[name], min);
        if (floatDistributionMaxs.find(name) == floatDistributionMaxs.end())
            floatDistributionMaxs[name] = max;
        else
            floatDistributionMaxs[name] =
            std::max(floatDistributionMaxs[name], max);
    }
    void ReportPercentage(const std::string& name, int64_t num, int64_t denom) {
        percentages[name].first += num;
        percentages[name].second += denom;
    }
    void ReportRatio(const std::string& name, int64_t num, int64_t denom) {
        ratios[name].first += num;
        ratios[name].second += denom;
    }

    void Print(FILE* file);
    void Clear();

private:
    // StatsAccumulator Private Data
    std::map<std::string, int64_t> counters;
    std::map<std::string, int64_t> memoryCounters;
    std::map<std::string, int64_t> intDistributionSums;
    std::map<std::string, int64_t> intDistributionCounts;
    std::map<std::string, int64_t> intDistributionMins;
    std::map<std::string, int64_t> intDistributionMaxs;
    std::map<std::string, double> floatDistributionSums;
    std::map<std::string, int64_t> floatDistributionCounts;
    std::map<std::string, double> floatDistributionMins;
    std::map<std::string, double> floatDistributionMaxs;
    std::map<std::string, std::pair<int64_t, int64_t>> percentages;
    std::map<std::string, std::pair<int64_t, int64_t>> ratios;
};

enum class Prof {
    SceneConstruction,
    AccelConstruction,
    TextureLoading,
    MIPMapCreation,

    IntegratorRender,
    MonteCarloIntegratorLi,
    SPPMCameraPass,
    SPPMGridConstruction,
    SPPMPhotonPass,
    SPPMStatsUpdate,
    BDPTGenerateSubpath,
    BDPTConnectSubpaths,
    LightDistribLookup,
    LightDistribSpinWait,
    LightDistribCreation,
    DirectLighting,
    halfDL,
    BSDFEvaluation,
    BSDFSampling,
    BSDFPdf,
    BSSRDFEvaluation,
    BSSRDFSampling,
    PhaseFuncEvaluation,
    PhaseFuncSampling,
    sceneRayIntersectTr,
    sceneRayIntersect,
    sceneRayOccluded,
    AccelRayIntersect,
    AccelRayOccluded,
    LightSample,
    shapePdfDir,
    MediumSample,
    MediumTr,
    TriRayIntersect,
    TriRayOccluded,
    CurveRayIntersect,
    CurveRayOccluded,
    ShapeRayIntersect,
    ShapeRayOccluded,
    ComputeScatteringFuncs,
    GenerateCameraRay,
    triFillSurfaceInteraction,
    siComputeDifferentials,
    MergeFilmTile,
    SplatFilm,
    AddFilmSample,
    StartPixel,
    GetSample,
    TexFiltTrilerp,
    TexFiltEWA,
    TexFiltPtex,
    NumProfCategories
};

static_assert((int)Prof::NumProfCategories <= 64,
    "No more than 64 profiling categories may be defined.");

inline uint64_t ProfToBits(Prof p) { return 1ull << (int)p; }

static const char* ProfNames[] = {
    "Scene parsing and creation",
    "Acceleration structure creation",
    "Texture loading",
    "MIP map generation",

    "Integrator::render()",
    "MonteCarloIntegrator::Li()",
    "SPPM camera pass",
    "SPPM grid construction",
    "SPPM photon pass",
    "SPPM photon statistics update",
    "BDPT subpath generation",
    "BDPT subpath connections",
    "SpatialLightDistribution lookup",
    "SpatialLightDistribution spin wait",
    "SpatialLightDistribution creation",
    "Direct lighting",
    "halfDL",
    "BSDF::f()",
    "BSDF::sample_f()",
    "BSDF::pdf()",
    "BSSRDF::f()",
    "BSSRDF::sample_f()",
    "PhaseFunction::p()",
    "PhaseFunction::sample_p()",
    "Scene::rayIntersectTr()",
    "Scene::rayIntersect()",
    "Scene::rayOccluded()",
    "Accelerator::rayIntersect()",
    "Accelerator::rayOccluded()",
    "Light::sample_*()",
    "Shape::pdfDir()",
    "Medium::sample()",
    "Medium::Tr()",
    "Triangle::rayIntersect()",
    "Triangle::rayOccluded()",
    "Curve::rayIntersect()",
    "Curve::rayOccluded()",
    "Other Shape::rayIntersect()",
    "Other Shape::rayOccluded()",
    "SurfaceInteraction::computeScatteringFunctions()",
    "Camera::generateRay[Differential]()",
    "TriangleI::fillSurfaceInteraction()",
    "SurfaceInteraction::computeDifferentials()",
    "Film::mergeTile()",
    "Film::addSplat()",
    "Film::addSample()",
    "Sampler::startPixelSample()",
    "Sampler::getSample[12]D()",
    "MIPMap::lookup() (trilinear)",
    "MIPMap::lookup() (EWA)",
    "Ptex lookup",
};

static_assert((int)Prof::NumProfCategories ==
    sizeof(ProfNames) / sizeof(ProfNames[0]),
    "ProfNames[] array and Prof enumerant have different "
    "numbers of entries!");

extern THREAD_LOCAL uint64_t ProfilerState;
inline uint64_t CurrentProfilerState() { return ProfilerState; }

class ProfilePhase {
public:
    // ProfilePhase Public Methods
    ProfilePhase(Prof p) {
        categoryBit = ProfToBits(p);
        reset = (ProfilerState & categoryBit) == 0;
        ProfilerState |= categoryBit;
    }
    ~ProfilePhase() {
        if (reset) ProfilerState &= ~categoryBit;
    }
    ProfilePhase(const ProfilePhase&) = delete;
    ProfilePhase& operator=(const ProfilePhase&) = delete;

private:
    // ProfilePhase Private Data
    bool reset;
    uint64_t categoryBit;
};

void InitProfiler();
void SuspendProfiler();
void ResumeProfiler();
void ProfilerWorkerThreadInit();
void ReportProfilerResults(FILE* dest);
void ClearProfiler();
void CleanupProfiler();

RENDERING_END