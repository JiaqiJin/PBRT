#include "bvh.hpp"

PALADIN_BEGIN

BVHAccel::BVHAccel(std::vector<std::shared_ptr<Primitive>> p,
    int maxPrimsInNode, SplitMethod splitMethod)
    : _maxPrimsInNode(std::min(255, maxPrimsInNode)),
    _splitMethod(splitMethod),
    _primitives(std::move(p))
{
    if (_primitives.empty()) return;
    // Build BVH from _primitives_

    // Initialize __primitiveInfo_ array for primitives
    std::vector<BVHPrimitiveInfo> _primitiveInfo(_primitives.size());
    for (size_t i = 0; i < _primitives.size(); ++i)
        _primitiveInfo[i] = { i, _primitives[i]->worldBound() };

    // Build BVH tree for primitives using __primitiveInfo_
    MemoryArena arena(1024 * 1024);
    int totalNodes = 0;
    std::vector<std::shared_ptr<Primitive>> orderedPrims;
    orderedPrims.reserve(_primitives.size());
    BVHBuildNode* root;
    if (splitMethod == SplitMethod::HLBVH)
        root = HLBVHBuild(arena, _primitiveInfo, &totalNodes, orderedPrims);
    else
        root = recursiveBuild(arena, _primitiveInfo, 0, _primitives.size(),
            &totalNodes, orderedPrims);
    _primitives.swap(orderedPrims);
    COUT << StringPrintf("BVH created with %d nodes for %d "
        "primitives (%.2f MB), arena allocated %.2f MB",
        totalNodes, (int)_primitives.size(),
        float(totalNodes * sizeof(LinearBVHNode)) /
        (1024.f * 1024.f),
        float(arena.totalAllocated()) /
        (1024.f * 1024.f));
    // Compute representation of depth-first traversal of BVH tree
    _nodes = allocAligned<LinearBVHNode>(totalNodes);
    int offset = 0;
    flattenBVHTree(root, &offset);
    CHECK_EQ(totalNodes, offset);
}

BVHBuildNode* BVHAccel::recursiveBuild(paladin::MemoryArena& arena, std::vector<BVHPrimitiveInfo>& primitiveInfo, 
    int start, int end, int* totalNodes, std::vector<std::shared_ptr<Primitive> >& orderedPrims) 
{
    return NULL;
}

bool BVHAccel::intersect(const Ray& ray, SurfaceInteraction* isect) const
{

}

bool BVHAccel::intersectP(const Ray& ray) const
{

}

PALADIN_END