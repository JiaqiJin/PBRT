#ifndef shape_hpp
#define shape_hpp

#include "header.h"
#include "../math/transform.hpp"
#include "interaction.hpp"

PALADIN_BEGIN
/*
所有图形的基类，假设读者已经掌握概率论基础知识
科普一下，pdf为概率密度函数，pdf在定义域上积分为1，且恒不小于零
如果没有读过概率密度函数，建议阅读浙大的概率密度函数，经典
shape类只是一个形状，不是一个具体的物体，具体的物体是primitive的子类，有材质，形状，是否发光等属性
*/
class Shape {
public:
    Shape(const Transform* ObjectToWorld, const Transform* WorldToObject,
        bool reverseOrientation);

    virtual ~Shape();

    // 返回在对象坐标系中的包围盒
    virtual AABB3f objectBound() const = 0;

    // 返回在世界坐标系中的包围盒
    virtual AABB3f worldBound() const;

    // 初始化函数，每个子类构造时都要调用
    // 目前用于计算表面积
    virtual void init() = 0;

    // 求交函数，填充SurfaceInteraction数据
    // 几乎所有的shape与ray求交的计算都是将ray转换到object空间中进行的
    virtual bool intersect(const Ray& ray,
        Float* tHit,
        SurfaceInteraction* isect,
        bool testAlphaTexture = true) const = 0;

    virtual bool intersectP(const Ray& ray,
        bool testAlphaTexture = true) const {
        return intersect(ray, nullptr, nullptr, testAlphaTexture);
    }

    // 表面积
    virtual Float area() const = 0;

    /*
    在图形表面采样一个点，返回该点的基于面积的概率密度函数值，与世界坐标中interaction结构
    u为表面参数坐标，u ∈ [0, 1]^2，通常为外部通过某种算法生成，最简单就是随机数
    基于面积的采样
    */
    virtual Interaction sampleA(const Point2f& u, Float* pdf) const = 0;

    // 概率密度函数，表面某点的pdf，函数空间为表面参数空间
    virtual Float pdfA(const Interaction& isect) const {
        return 1 / area();
    }

    // 概率密度函数，表面某点的pdf，函数空间为表面参数空间
    virtual Float pdfA() const {
        return 1 / area();
    }

    /*
     在场景的某处ref处，随机采样shape，返回世界坐标中interaction结构
     以及在ref处采样到对应shape表面点的概率密度函数
     基于立体角的采样
    */
    virtual Interaction sampleW(const Interaction& ref, const Point2f& u, Float* pdf) const;

    /*
    在场景中某处ref处沿着wi方向采样图形上某点的概率密度函数
    函数空间为向量空间
    */
    virtual Float pdfW(const Interaction& ref, const Vector3f& wi) const;

    /*
      返回shape对于某个点的立体角大小
     */
    virtual Float solidAngle(const Point3f& p, int nSamples = 512) const;

    const Transform* objectToWorld;
    const Transform* worldToObject;
    const bool reverseOrientation;
    const bool transformSwapsHandedness;
protected:
    // 参照mitsuba渲染器，后续优化，保存表面积的倒数，每次图形有变化时更新数据
    // 初始值为零，0为非法值
    Float _invArea = 0;
};

PALADIN_END

#endif /* shape_hpp */