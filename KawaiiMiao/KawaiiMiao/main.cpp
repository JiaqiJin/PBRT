#include "Core/Scene.h"
#include "Core/Light.h"
#include "Core/Shape.h"
#include "Core/Primitive.h"
#include "Core/Material.h"
#include "Core/Integrator.h"

using namespace Render;
using namespace std;

int main()
{
	Render::Log::Init();

	Float white[] = { 0.73f, 0.73f, 0.73f };
	Float green[] = { 0.12f, 0.45f, 0.15f };
	Float red[] = { 0.65f, 0.05f, 0.05f };
	Float blue[] = { 0.05f, 0.05f, 0.75f };
	Float light[] = { 4.0f, 4.0f, 4.0f };

	const Spectrum white_unit = Spectrum::fromRGB(white);
	const Spectrum green_unit = Spectrum::fromRGB(green);
	const Spectrum red_unit = Spectrum::fromRGB(red);
	const Spectrum blue_unit = Spectrum::fromRGB(blue);
	const Spectrum light_unit = Spectrum::fromRGB(light);

	//Material
	Material::ptr whiteLambert_mat = std::make_shared<LambertianMaterial>(white_unit);
	Material::ptr greenLambert_mat = std::make_shared<LambertianMaterial>(green_unit);
	Material::ptr blueLambert_mat = std::make_shared<MirrorMaterial>(blue_unit);
	Material::ptr redLambert_mat = std::make_shared<LambertianMaterial>(red_unit);

	Transform trans1 = translate(Vector3f(+1.5, 1.5, +2));
	SphereShape::ptr sphere1 = std::make_shared<SphereShape>(trans1, inverse(trans1), 1.5f);
	HitableObject::ptr entity1 = std::make_shared<HitableObject>(sphere1, blueLambert_mat, nullptr);

	Transform trans2 = translate(Vector3f(-1.5, 2.5, +0));
	SphereShape::ptr sphere2 = std::make_shared<SphereShape>(trans2, inverse(trans2), 2.5f);
	HitableObject::ptr entity2 = std::make_shared<HitableObject>(sphere2, whiteLambert_mat, nullptr);

	//Bottom wall
	Transform identity(Matrix4x4(1.0f));
	Vector3f t1[3] = { Vector3f(-5, 0, -5), Vector3f(-5, 0, +5), Vector3f(+5, 0, -5) };
	TriangleShape::ptr bot1 = std::make_shared<TriangleShape>(identity, identity, t1);
	HitableObject::ptr entity3 = std::make_shared<HitableObject>(bot1, whiteLambert_mat, nullptr);

	Vector3f t2[3] = { Vector3f(+5, 0, -5), Vector3f(-5, 0, +5), Vector3f(+5, 0, +5) };
	TriangleShape::ptr bot2 = std::make_shared<TriangleShape>(identity, identity, t2);
	HitableObject::ptr entity4 = std::make_shared<HitableObject>(bot2, whiteLambert_mat, nullptr);

	//Top wall
	Vector3f t3[3] = { Vector3f(-5, 10, -5), Vector3f(+5, 10, -5), Vector3f(-5, 10, +5) };
	TriangleShape::ptr top1 = std::make_shared<TriangleShape>(identity, identity, t3);
	HitableObject::ptr entity5 = std::make_shared<HitableObject>(top1, whiteLambert_mat, nullptr);

	Vector3f t4[3] = { Vector3f(+5, 10, -5), Vector3f(+5, 10, +5), Vector3f(-5, 10, +5) };
	TriangleShape::ptr top2 = std::make_shared<TriangleShape>(identity, identity, t4);
	HitableObject::ptr entity6 = std::make_shared<HitableObject>(top2, whiteLambert_mat, nullptr);

	//Left wall
	Vector3f t5[3] = { Vector3f(-5, 10, +5), Vector3f(-5, 10, -5), Vector3f(-5, 0, +5) };
	TriangleShape::ptr lef1 = std::make_shared<TriangleShape>(identity, identity, t5);
	HitableObject::ptr entity7 = std::make_shared<HitableObject>(lef1, greenLambert_mat, nullptr);

	Vector3f t6[3] = { Vector3f(-5, 10, -5), Vector3f(-5, 0, -5), Vector3f(-5, 0, +5) };
	TriangleShape::ptr lef2 = std::make_shared<TriangleShape>(identity, identity, t6);
	HitableObject::ptr entity8 = std::make_shared<HitableObject>(lef2, greenLambert_mat, nullptr);

	//Right wall
	Vector3f t7[3] = { Vector3f(+5, 10, +5), Vector3f(+5, 10, -5), Vector3f(+5, 0, +5) };
	TriangleShape::ptr rig1 = std::make_shared<TriangleShape>(identity, identity, t7);
	HitableObject::ptr entity9 = std::make_shared<HitableObject>(rig1, redLambert_mat, nullptr);

	Vector3f t8[3] = { Vector3f(+5, 10, -5), Vector3f(+5, 0, -5), Vector3f(+5, 0, +5) };
	TriangleShape::ptr rig2 = std::make_shared<TriangleShape>(identity, identity, t8);
	HitableObject::ptr entity10 = std::make_shared<HitableObject>(rig2, redLambert_mat, nullptr);

	//Back wall
	Vector3f t9[3] = { Vector3f(-5, 10, -5), Vector3f(+5, 10, -5), Vector3f(-5, 0, -5) };
	TriangleShape::ptr bak1 = std::make_shared<TriangleShape>(identity, identity, t9);
	HitableObject::ptr entity11 = std::make_shared<HitableObject>(bak1, whiteLambert_mat, nullptr);

	Vector3f t10[3] = { Vector3f(+5, 10, -5), Vector3f(+5, 0, -5), Vector3f(-5, 0, -5) };
	TriangleShape::ptr bak2 = std::make_shared<TriangleShape>(identity, identity, t10);
	HitableObject::ptr entity12 = std::make_shared<HitableObject>(bak2, whiteLambert_mat, nullptr);

	//light
	Vector3f t11[3] = { Vector3f(-2, 10, -2), Vector3f(+2, 10, -2), Vector3f(-2, 10, +2) };
	TriangleShape::ptr lamp1 = std::make_shared<TriangleShape>(identity, identity, t11);
	AreaLight::ptr light1 = std::make_shared<DiffuseAreaLight>(identity, light_unit, 8, lamp1);
	HitableObject::ptr entity13 = std::make_shared<HitableObject>(lamp1, whiteLambert_mat, light1);

	Vector3f t12[3] = { Vector3f(+2, 10, -2), Vector3f(+2, 10, +2), Vector3f(-2, 10, +2) };
	TriangleShape::ptr lamp2 = std::make_shared<TriangleShape>(identity, identity, t12);
	AreaLight::ptr light2 = std::make_shared<DiffuseAreaLight>(identity, light_unit, 8, lamp2);
	HitableObject::ptr entity14 = std::make_shared<HitableObject>(lamp2, whiteLambert_mat, light2);

	HitableList::ptr aggregate = std::make_shared<HitableList>();
	aggregate->addHitable(entity1);
	aggregate->addHitable(entity2);
	aggregate->addHitable(entity3);
	aggregate->addHitable(entity4);
	aggregate->addHitable(entity5);
	aggregate->addHitable(entity6);
	aggregate->addHitable(entity7);
	aggregate->addHitable(entity8);
	aggregate->addHitable(entity9);
	aggregate->addHitable(entity10);
	aggregate->addHitable(entity11);
	aggregate->addHitable(entity12);
	aggregate->addHitable(entity13);
	aggregate->addHitable(entity14);
	//aggregate->addHitable(entity15);

	//Scene
	std::vector<Light::ptr> lights = { light1, light2 };
	Scene::ptr scene = std::make_shared<Scene>(aggregate, lights);

	int maxDepth = 4;
	int width = 666, height = 500;

	//Film & sampler
	Vector2i res(width, height);
	std::unique_ptr<Filter> filter(new GaussianFilter(Vector2f(2.0f, 2.0f),2.0f));
	Film::ptr film = std::make_shared<Film>(res, Bounds2f(Vector2f(0, 0), Vector2f(1, 1)),
		std::move(filter), "../result.png");
	Sampler::ptr sampler = std::make_shared<RandomSampler>(8);

	Float fovy = 45.0f;
	Vector3f eye(0, 5, 18), center(0, 5, 0);

	//Camera
	Bounds2f screen;
	Float frame = (Float)(width) / height;
	if (frame > 1.f)
	{
		screen.m_pMin.x = -frame;
		screen.m_pMax.x = frame;
		screen.m_pMin.y = -1.f;
		screen.m_pMax.y = 1.f;
	}
	else
	{
		screen.m_pMin.x = -1.f;
		screen.m_pMax.x = 1.f;
		screen.m_pMin.y = -1.f / frame;
		screen.m_pMax.y = 1.f / frame;
	}
	auto cameraToWorld = inverse(lookAt(eye, center, Vector3f(0, 1, 0)));
	Camera::ptr camera = std::make_shared<PerspectiveCamera>(cameraToWorld, screen, fovy, film);

	Bounds2i pixelBound(Vector2i(0, 0), Vector2i(width, height));

	WhittedIntegrator integrator(maxDepth, camera, sampler, pixelBound);

	printf("Kawaii (built %s at %s) [Detected %d cores]\n",
		__DATE__, __TIME__, numSystemCores());

	integrator.preprocess(*scene, *sampler);
	integrator.render(*scene);

	return 0;
}

/*
Scene
*/