#ifndef camera_hpp
#define camera_hpp

#include "header.h"
#include "film.hpp"
#include "../math/animatedtransform.hpp"

PALADIN_BEGIN

// 相机样本
struct CameraSample
{
	//胶片上的点，产生的射线将其辐射。
	Point2f pFilm;
	//光线穿过镜头上的点
	Point2f pLens;
	//采样时间
	Float time;
};

/*
 相机空间（camera space）：以相机在世界空间中的位置为原点，相机面朝的方向为 z 轴正方向展开的三维空间。

 屏幕空间（screen space）：屏幕空间定义在像平面（也可以说是近平面,胶片平面）上，屏幕空间中近平面所在矩形中点的坐标为 (0,0)
 ，坐标轴单位长度由长和宽中较短的边决定。相机空间物体将被投射到像平面上，而在屏幕窗口内可见的部分最终会出现在图像中。
 在屏幕空间下，深度值 z的范围是 [0,1] ，0和1分别对应了近平面和远平面上的物体。
 尽管屏幕空间是定义在像平面上的，但它仍然是一个三维空间，因为在该空间下坐标分量 [公式] 仍然是有意义的
 标准化设备坐标空间（normalized device coordinate space，NDC space）:xyz三个维度都在[0,1]范围内，(0,0)为左上角

 光栅空间（raster space）：该空间与NDC空间基本相同，z范围是[0,1]，唯一的区别 x范围是[0,res.x]，y范围是[0,res.y]
*/
class Camera
{
public:
	Camera(const AnimatedTransform& CameraToWorld, Float shutterOpen,
		Float shutterClose, Film* film, const Medium* medium);

	virtual ~Camera() { delete film; }

	/*
	生成ray中的dir分量为单位向量
	表示有多少沿着该光线有多少radiance到达相平面上
	一般返回值为0或1，真实相机中可能返回其他值
	*/
	virtual Float generateRay(const CameraSample& sample, Ray* ray) const = 0;

	/*
	返回值为一个浮点数，表示有多少沿着该光线有多少radiance到达相平面上
	一般返回值为0或1，真实相机中可能返回其他值
	但还会计算在胶片平面上在x和y方向偏移一个像素的相应光线
	*/
	virtual Float generateRayDifferential(const CameraSample& sample,
		RayDifferential* rd) const;

	virtual Spectrum we(const Ray& ray, Point2f* pRaster2 = nullptr) const;

	virtual void pdfWe(const Ray& ray, Float* pdfPos, Float* pdfDir) const;

	virtual Spectrum sampleWi(const Interaction& ref, const Point2f& u,
		Vector3f* wi, Float* pdf, Point2f* pRaster,
		VisibilityTester* vis) const;

	//相机空间到世界空间的转换，用animatetransform可以做动态模糊
	AnimatedTransform cameraToWorld;
	// 快门开启时间，快门关闭时间
	const Float shutterOpen, shutterClose;
	// 胶片
	Film* film;
	// 相机所在的介质
	const Medium* medium;
};

class ProjectiveCamera : public Camera 
{
public:

protected:

};


PALADIN_END

#endif /* camera_hpp */
