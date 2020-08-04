#ifndef shape_hpp
#define shape_hpp

#include "header.h"
#include "../math/transform.hpp"
#include "interaction.hpp"

KAWAII_BEGIN

class Shape
{
public:
	Shape(const Transform* ObjectToWorld, const Transform* WorldToObject,
		bool reverseOrientation);

	virtual ~Shape();

	// 返回在对象坐标系中的包围盒
	virtual Bounds3f objectBound() const = 0;

	// 返回在世界坐标系中的包围盒
	virtual Bounds3f worldBound() const;

	// 求交函数(intersect function)，填充SurfaceInteraction数据
	virtual bool intersect(const Ray& ray,
		Float* tHit,
		SurfaceInteraction* isect,
		bool testAlphaTexture = true) const = 0;

	virtual bool IntersectP(const Ray& ray,
		bool testAlphaTexture = true) const {
		return intersect(ray, nullptr, nullptr, testAlphaTexture);
	}

	// 表面积
	virtual Float area() const = 0;

	/*
在图形表面采样一个点，返回该点的概率密度函数，与interaction结构
u为表面参数坐标，u ∈ [0, 1]^2，通常为外部通过某种算法生成，最简单就是随机数
*/
	virtual Interaction sample(const Point2f& u, Float* pdf) const = 0;

	// 概率密度函数，表面某点的pdf，函数空间为表面参数空间
	virtual Float pdf(const Interaction& isect) const {
		return 1 / area();
	}

	/*
	 在场景的某处ref处，随机采样shape，返回interaction结构
	 以及在ref处采样到对应shape表面点的概率密度函数
	*/
	virtual Interaction sample(const Interaction& ref, const Point2f& u, Float* pdf) const;

	/*
	在场景中某处ref处沿着wi方向采样图形上某点的概率密度函数
	函数空间为向量空间
	*/
	virtual Float pdf(const Interaction& ref, const Vector3f& wi) const;

	virtual Float SolidAngle(const Point3f& p, int nSamples = 512) const;



	//Transformation thaht describe the mapping from object space to world space
	const Transform* _objectToWorld;
	const Transform* _worldToObject;
	//Indicate where the surface nornal sould be reverese
	const bool _reverseOrientation;
	//world to object transformation
	const bool _transformSwapsHandedness;

};

KAWAII_END

#endif /* shape_hpp */
