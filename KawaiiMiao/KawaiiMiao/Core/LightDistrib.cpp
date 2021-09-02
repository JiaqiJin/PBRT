#include "LightDistrib.h"
#include "Scene.h"

RENDER_BEGIN

std::unique_ptr<LightDistribution> createLightSampleDistribution(
	const std::string& name, const Scene& scene)
{
	//if (name == "uniform" || scene.m_lights.size() == 1)
	{
		return std::unique_ptr<LightDistribution>{
			new UniformLightDistribution(scene)};
	}
	//else if (name == "power")
	//{
	//	return std::unique_ptr<ALightDistribution>{
	//		new APowerLightDistribution(scene)};
	//}
	//else if (name == "spatial")
	//{
	//	return std::unique_ptr<ALightDistribution>{
	//		new ASpatialLightDistribution(scene)};
	//}
	//else 
	//{
	//	Error(
	//		"Light sample distribution type \"%s\" unknown. Using \"spatial\".",
	//		name.c_str());
	//	return std::unique_ptr<LightDistribution>{new SpatialLightDistribution(scene)};
	//}
}

UniformLightDistribution::UniformLightDistribution(const Scene& scene)
{
	std::vector<Float> prob(scene.m_lights.size(), Float(1));
	distrib.reset(new Distribution1D(&prob[0], int(prob.size())));
}

const Distribution1D* UniformLightDistribution::lookup(const Vector3f& p) const
{
	return distrib.get();
}

RENDER_END