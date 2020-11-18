﻿#include "triangle.hpp"
#include "sampling.hpp"

PALADIN_BEGIN

TriangleMesh::TriangleMesh(
    const Transform& ObjectToWorld, int nTriangles, const int* vertexIndices,
    int nVertices, const Point3f* P, const Vector3f* S, const Normal3f* N,
    const Point2f* UV, const std::shared_ptr<Texture<Float>>& alphaMask,
    const std::shared_ptr<Texture<Float>>& shadowAlphaMask,
    const int* fIndices)
    : nTriangles(nTriangles),
    nVertices(nVertices),
    vertexIndices(vertexIndices, vertexIndices + 3 * nTriangles),
    alphaMask(alphaMask),
    shadowAlphaMask(shadowAlphaMask) {


    points.reset(new Point3f[nVertices]);
    for (int i = 0; i < nVertices; ++i) {
        points[i] = ObjectToWorld.exec(P[i]);
    }

    if (UV) {
        uv.reset(new Point2f[nVertices]);
        memcpy(uv.get(), UV, nVertices * sizeof(Point2f));
    }
    if (N) {
        normals.reset(new Normal3f[nVertices]);
        for (int i = 0; i < nVertices; ++i) {
            normals[i] = ObjectToWorld.exec(N[i]);
        }
    }
    if (S) {
        edges.reset(new Vector3f[nVertices]);
        for (int i = 0; i < nVertices; ++i) {
            edges[i] = ObjectToWorld.exec(S[i]);
        }
    }

    if (fIndices) {
        faceIndices = std::vector<int>(fIndices, fIndices + nTriangles);
    }
}

AABB3f Triangle::objectBound() const {
    const Point3f& p0 = _mesh->points[_vertexIdx[0]];
    const Point3f& p1 = _mesh->points[_vertexIdx[1]];
    const Point3f& p2 = _mesh->points[_vertexIdx[2]];
    AABB3f b1 = AABB3f(worldToObject->exec(p0), worldToObject->exec(p1));
    return unionSet(b1, worldToObject->exec(p2));;
}

bool Triangle::intersect(const Ray& ray, Float* tHit, SurfaceInteraction* isect,
    bool testAlphaTexture) const {
    const Point3f& p0 = _mesh->points[_vertexIdx[0]];
    const Point3f& p1 = _mesh->points[_vertexIdx[1]];
    const Point3f& p2 = _mesh->points[_vertexIdx[2]];

    Float u, v, t;

    Vector3f edge1 = p1 - p0;
    Vector3f edge2 = p2 - p0;

    Vector3f pvec = cross(ray.dir, edge2);

    Float det = dot(edge1, pvec);

    if (det == 0) {
        // ray与三角形所在平面平行
        return false;
    }
    Float invDet = 1 / det;
    Vector3f tvec = ray.ori - p0;

    u = dot(tvec, pvec) * invDet;
    if (u < 0.0 || u > 1.0) {
        return false;
    }

    Vector3f qvec = cross(tvec, edge1);

    v = dot(ray.dir, qvec) * invDet;

    if (v >= 0.0 && u + v <= 1.0) {
        t = dot(edge2, qvec) * invDet;
        if (t > ray.tMax) {
            return false;
        }
        *tHit = t;
    }

    // 计算微分几何
    // 三角形中的点可以表示为
    // pi = p0 + ui * dp/du + vi * dp/dv
    // 求解偏导数dp/du dp/dv 矩阵方程如下
    // |u0 - u2  v0 - v2|   |dp/du|   |p0 - p2|
    // |                | * |     | = |       |
    // |u1 - u2  v1 - v2|   |dp/dv|   |p1 - p2|
    // |dp/du|   |u0 - u2  v0 - v2| -1    |p0 - p2|
    // |     | = |                |    *  |       |
    // |dp/dv|   |u1 - u2  v1 - v2|       |p1 - p2|
    Point3f pHit = u * p0 + v * p1 + (1 - u - v) * p2;
    Vector3f dpdu, dpdv;
    Point2f uv[3];
    getUVs(uv);
    Vector2f duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
    Vector3f dp02 = p0 - p2, dp12 = p1 - p2;
    Float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
    bool degenerateUV = std::abs(determinant) < 1e-8;

    if (!degenerateUV) {
        Float invdet = 1 / determinant;
        dpdu = (duv12[1] * dp02 - duv02[1] * dp12) * invdet;
        dpdv = (-duv12[0] * dp02 + duv02[0] * dp12) * invdet;
    }
    if (degenerateUV || cross(dpdu, dpdv).lengthSquared() == 0) {
        Vector3f ng = cross(edge2, edge1);
        if (ng.lengthSquared() == 0) {
            return false;
        }
        coordinateSystem(normalize(ng), &dpdu, &dpdv);
    }

    // 保守估计误差todo 后续补上推导过程
    Vector3f pError = gamma(6) * Vector3f(pHit);

    *isect = SurfaceInteraction(pHit, pError, Point2f(u, v), -ray.dir, dpdu, dpdv,
        Normal3f(0, 0, 0), Normal3f(0, 0, 0), ray.time,
        this, _faceIndex);

    isect->normal = isect->shading.normal = Normal3f(normalize(cross(dp02, dp12)));
    // todo 临时做个简单的，着色几何信息与实际信息一致
    isect->setShadingGeometry(dpdu, dpdv, Normal3f(), Normal3f(), true);
    if (_mesh->normals) {
        isect->normal = faceforward(isect->normal, isect->shading.normal);
    }
    else if (reverseOrientation ^ transformSwapsHandedness) {
        isect->normal = isect->shading.normal = -isect->normal;
    }
    return true;
}

/*
 基本思路
 三角形的参数方程如下
 u * p0 + v * p1 + (1 - u - v) * p2;
 其中p0，p1和p2是三角形的三个点，u, v是p1和p2的权重，1-u-v是p0的权重，并且满足u>=0, v >= 0,u+v<=1
 ray的方程为o + td
 联合以上两个方程得
 u(p1 - p0) + v(p2 - p0) - td = o - p0
 e1 = p1 - p0
 e2 = p2 - p0
 ue1 + ve2 - td = o - p0
 改写成如下形式
                 t
 [-d  e1  e2] * [u] = T
                 v
 求解上述方程组
 求得u>=0, v >= 0,u+v<=1，t <= tMax则有交点
*/
bool Triangle::intersectP(const Ray& ray, bool testAlphaTexture) const {
    const Point3f& p0 = _mesh->points[_vertexIdx[0]];
    const Point3f& p1 = _mesh->points[_vertexIdx[1]];
    const Point3f& p2 = _mesh->points[_vertexIdx[2]];

    Float u, v, t;

    Vector3f edge1 = p1 - p0;
    Vector3f edge2 = p2 - p0;

    Vector3f pvec = cross(ray.dir, edge2);

    Float det = dot(edge1, pvec);

    if (det == 0) {
        // ray与三角形所在平面平行
        return false;
    }
    Float invDet = 1 / det;
    Vector3f tvec = ray.ori - p0;

    u = dot(tvec, pvec) * invDet;
    if (u < 0.0 || u > 1.0) {
        return false;
    }

    Vector3f qvec = cross(tvec, edge1);

    v = dot(ray.dir, qvec) * invDet;

    if (v >= 0.0 && u + v <= 1.0) {
        t = dot(edge2, qvec) * invDet;
        return t <= ray.tMax;
    }

    return true;
}

AABB3f Triangle::worldBound() const {
    const Point3f& p0 = _mesh->points[_vertexIdx[0]];
    const Point3f& p1 = _mesh->points[_vertexIdx[1]];
    const Point3f& p2 = _mesh->points[_vertexIdx[2]];
    AABB3f b1 = AABB3f(p0, p1);
    return unionSet(b1, p2);
}

Interaction Triangle::sampleA(const Point2f& u, Float* pdf) const {
    Interaction ret;
    Point2f b = uniformSampleTriangle(u);
    const Point3f& p0 = _mesh->points[_vertexIdx[0]];
    const Point3f& p1 = _mesh->points[_vertexIdx[1]];
    const Point3f& p2 = _mesh->points[_vertexIdx[2]];
    // 2D三角形坐标转换为3D空间三角形坐标
    ret.pos = b[0] * p0 + b[1] * p1 + (1 - b[0] - b[1]) * p2;

    ret.normal = normalize(Normal3f(cross(p1 - p0, p2 - p0)));

    if (_mesh->normals) {
        Normal3f ns(b[0] * _mesh->normals[_vertexIdx[0]] + b[1] * _mesh->normals[_vertexIdx[1]] +
            (1 - b[0] - b[1]) * _mesh->normals[_vertexIdx[2]]);
        ret.normal = faceforward(ret.normal, ns);
    }
    else if (reverseOrientation ^ transformSwapsHandedness) {
        ret.normal *= -1;
    }
    Point3f pAbsSum = abs(b[0] * p0) + abs(b[1] * p1) + abs((1 - b[0] - b[1]) * p2);
    //todo 推导过程后续补上
    ret.pError = gamma(6) * Vector3f(pAbsSum.x, pAbsSum.y, pAbsSum.z);
    *pdf = pdfA();
    return ret;
}

PALADIN_END