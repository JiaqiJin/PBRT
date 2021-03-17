#pragma once

#include "../core/Header.h"
#include "../core/primitive.h"
#include "../core/Shape.h"

RENDERING_BEGIN

struct BVHPrimitiveInfo {
    BVHPrimitiveInfo() {}
    BVHPrimitiveInfo(size_t primitiveNumber, const AABB3f& bounds)
        : primitiveNumber(primitiveNumber),
        bounds(bounds),
        centroid(.5f * bounds.pMin + .5f * bounds.pMax) {}
    size_t primitiveNumber;
    AABB3f bounds;
    Point3f centroid;
};

struct BVHBuildNode {
    void InitLeaf(int first, int n, const AABB3f& b) {
        firstPrimOffset = first;
        nPrimitives = n;
        bounds = b;
        children[0] = children[1] = nullptr;
    }

    void initInterior(int axis, BVHBuildNode* c0, BVHBuildNode* c1) {
        children[0] = c0;
        children[1] = c1;
        bounds = unionSet(c0->bounds, c1->bounds);
        splitAxis = axis;
        nPrimitives = 0;
    }

    AABB3f bounds;
    // 二叉树的两个子节点
    BVHBuildNode* children[2];
    // 分割的坐标轴
    int splitAxis;
    // 第一个片元的偏移量
    int firstPrimOffset;
    // 片元数量
    int nPrimitives;
};

struct MortonPrimitive {
    int primitiveIndex;
    uint32_t mortonCode;
};

struct LBVHTreelet {
    int startIndex, nPrimitives;
    BVHBuildNode* buildNodes;
};

struct LinearBVHNode {
    AABB3f bounds;
    union {
        int primitivesOffset;   //指向图元
        int secondChildOffset;  // 第二个子节点在数组中的偏移量
    };
    uint16_t nPrimitives;  // 图元数量
    uint8_t axis;          // interior node: xyz
    uint8_t pad[1];        // 确保32个字节为一个对象，提高缓存命中率
};

struct BucketInfo {
    int count = 0;
    AABB3f bounds;
};

class BVHAccel : public Aggregate {
public:
    enum SplitMethod { SAH, HLBVH, Middle, EqualCounts };

    BVHAccel(std::vector<std::shared_ptr<Primitive>> p,
        int maxPrimsInNode = 1,
        SplitMethod splitMethod = SplitMethod::SAH);
    ~BVHAccel();
    virtual AABB3f worldBound() const override;
    virtual bool intersect(const Ray& r, SurfaceInteraction*) const override;
    virtual bool intersectP(const Ray& r) const override;
private:
    // BVHAccel Private Methods
    BVHBuildNode* recursiveBuild(
        MemoryArena& arena, std::vector<BVHPrimitiveInfo>& primitiveInfo,
        int start, int end, int* totalNodes,
        std::vector<std::shared_ptr<Primitive>>& orderedPrims);
    BVHBuildNode* HLBVHBuild(
        MemoryArena& arena, const std::vector<BVHPrimitiveInfo>& primitiveInfo,
        int* totalNodes,
        std::vector<std::shared_ptr<Primitive>>& orderedPrims) const;
    BVHBuildNode* emitLBVH(
        BVHBuildNode*& buildNodes,
        const std::vector<BVHPrimitiveInfo>& primitiveInfo,
        MortonPrimitive* mortonPrims, int nPrimitives, int* totalNodes,
        std::vector<std::shared_ptr<Primitive>>& orderedPrims,
        std::atomic<int>* orderedPrimsOffset, int bitIndex) const;
    BVHBuildNode* buildUpperSAH(MemoryArena& arena,
        std::vector<BVHBuildNode*>& treeletRoots,
        int start, int end, int* totalNodes) const;
    int flattenBVHTree(BVHBuildNode* node, int* offset);

    const int maxPrimsInNode;
    const SplitMethod splitMethod;
    std::vector<std::shared_ptr<Primitive>> primitives;
    LinearBVHNode* nodes = nullptr;
};

RENDERING_END