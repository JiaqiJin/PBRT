#include "bvh.h"

RENDERING_BEGIN

// BVHAccel Utility Functions
inline uint32_t LeftShift3(uint32_t x) {
    CHECK_LE(x, (1 << 10));
    if (x == (1 << 10)) --x;
#ifdef PBRT_HAVE_BINARY_CONSTANTS
    x = (x | (x << 16)) & 0b00000011000000000000000011111111;
    // x = ---- --98 ---- ---- ---- ---- 7654 3210
    x = (x | (x << 8)) & 0b00000011000000001111000000001111;
    // x = ---- --98 ---- ---- 7654 ---- ---- 3210
    x = (x | (x << 4)) & 0b00000011000011000011000011000011;
    // x = ---- --98 ---- 76-- --54 ---- 32-- --10
    x = (x | (x << 2)) & 0b00001001001001001001001001001001;
    // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
#else
    x = (x | (x << 16)) & 0x30000ff;
    // x = ---- --98 ---- ---- ---- ---- 7654 3210
    x = (x | (x << 8)) & 0x300f00f;
    // x = ---- --98 ---- ---- 7654 ---- ---- 3210
    x = (x | (x << 4)) & 0x30c30c3;
    // x = ---- --98 ---- 76-- --54 ---- 32-- --10
    x = (x | (x << 2)) & 0x9249249;
    // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
#endif // PBRT_HAVE_BINARY_CONSTANTS
    return x;
}

inline uint32_t EncodeMorton3(const Vector3f& v) {
    CHECK_GE(v.x, 0);
    CHECK_GE(v.y, 0);
    CHECK_GE(v.z, 0);
    return (LeftShift3(v.z) << 2) | (LeftShift3(v.y) << 1) | LeftShift3(v.x);
}

static void RadixSort(std::vector<MortonPrimitive>* v) {
    std::vector<MortonPrimitive> tempVector(v->size());
    CONSTEXPR int bitsPerPass = 6;
    CONSTEXPR int nBits = 30;
    static_assert((nBits % bitsPerPass) == 0,
        "Radix sort bitsPerPass must evenly divide nBits");
    CONSTEXPR int nPasses = nBits / bitsPerPass;

    for (int pass = 0; pass < nPasses; ++pass) {
        // Perform one pass of radix sort, sorting _bitsPerPass_ bits
        int lowBit = pass * bitsPerPass;

        // Set in and out vector pointers for radix sort pass
        std::vector<MortonPrimitive>& in = (pass & 1) ? tempVector : *v;
        std::vector<MortonPrimitive>& out = (pass & 1) ? *v : tempVector;

        // Count number of zero bits in array for current radix sort bit
        CONSTEXPR int nBuckets = 1 << bitsPerPass;
        int bucketCount[nBuckets] = { 0 };
        CONSTEXPR int bitMask = (1 << bitsPerPass) - 1;
        for (const MortonPrimitive& mp : in) {
            int bucket = (mp.mortonCode >> lowBit) & bitMask;
            CHECK_GE(bucket, 0);
            CHECK_LT(bucket, nBuckets);
            ++bucketCount[bucket];
        }

        // Compute starting index in output array for each bucket
        int outIndex[nBuckets];
        outIndex[0] = 0;
        for (int i = 1; i < nBuckets; ++i)
            outIndex[i] = outIndex[i - 1] + bucketCount[i - 1];

        // Store sorted values in output array
        for (const MortonPrimitive& mp : in) {
            int bucket = (mp.mortonCode >> lowBit) & bitMask;
            out[outIndex[bucket]++] = mp;
        }
    }
    // Copy final result from _tempVector_, if needed
    if (nPasses & 1) std::swap(*v, tempVector);
}

AABB3f BVHAccel::worldBound() const {
    return nodes ? nodes[0].bounds : AABB3f();
}


BVHAccel::BVHAccel(std::vector<std::shared_ptr<Primitive>> p,
    int maxPrimsInNode, SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)),
    splitMethod(splitMethod),
    primitives(std::move(p)) {

    if (primitives.empty()) return;

    // Initialize _primitiveInfo_ array for primitives
    std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
    for (size_t i = 0; i < primitives.size(); ++i)
        primitiveInfo[i] = { i, primitives[i]->worldBound() };

    MemoryArena arena(1024 * 1024);
    int totalNodes = 0;
    std::vector<std::shared_ptr<Primitive>> orderedPrims;
    orderedPrims.reserve(primitives.size());
    BVHBuildNode *root;
    if (splitMethod == SplitMethod::HLBVH)
        root = HLBVHBuild(arena, primitiveInfo, &totalNodes, orderedPrims);
    else
        root = recursiveBuild(arena, primitiveInfo, 0, primitives.size(),
            &totalNodes, orderedPrims);
    primitives.swap(orderedPrims);
    primitiveInfo.resize(0);
    
    nodes = allocAligned<LinearBVHNode>(totalNodes);
    int offset = 0;
    flattenBVHTree(root, &offset);
    CHECK_EQ(totalNodes, offset);
}

BVHAccel::~BVHAccel() {}

BVHBuildNode* BVHAccel::recursiveBuild(
    MemoryArena& arena, std::vector<BVHPrimitiveInfo>& primitiveInfo,
    int start, int end, int* totalNodes,
    std::vector<std::shared_ptr<Primitive>>& orderedPrims) {
    return nullptr;
}

BVHBuildNode* BVHAccel::HLBVHBuild(
    MemoryArena& arena, const std::vector<BVHPrimitiveInfo>& primitiveInfo,
    int* totalNodes,
    std::vector<std::shared_ptr<Primitive>>& orderedPrims) const {
    return nullptr;
}

BVHBuildNode* BVHAccel::emitLBVH(
    BVHBuildNode*& buildNodes,
    const std::vector<BVHPrimitiveInfo>& primitiveInfo,
    MortonPrimitive* mortonPrims, int nPrimitives, int* totalNodes,
    std::vector<std::shared_ptr<Primitive>>& orderedPrims,
    std::atomic<int>* orderedPrimsOffset, int bitIndex) const {
    return nullptr;
}

BVHBuildNode* BVHAccel::buildUpperSAH(MemoryArena& arena,
    std::vector<BVHBuildNode*>& treeletRoots,
    int start, int end, int* totalNodes) const {
    return nullptr;
}

int BVHAccel::flattenBVHTree(BVHBuildNode* node, int* offset) {
    return 1;
}

bool BVHAccel::intersect(const Ray& r, SurfaceInteraction*) const {
    return false;
}

bool BVHAccel::intersectP(const Ray& r) const {
    return false;
}

RENDERING_END