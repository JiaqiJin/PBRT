#include "Core/Scene.h"
#include "Core/Integrator.h"
#include "Core/SceneParser.h"

using namespace Render;
using namespace std;

int main()
{
	Render::Log::Init();

	//Banner
	{
		printf("Kawaii (built %s at %s) [Detected %d cores]\n", __DATE__, __TIME__, numSystemCores());
	}

	auto parsing_func = [](const std::string& filename)->void
	{
		
	};
	const std::string filename = "scenes/cornellBox/cornellBox.json";

	Scene::ptr scene = nullptr;
	Integrator::ptr integrator = nullptr;

	SceneParser::parser(filename, scene, integrator);

	CHECK_NE(scene, nullptr);
	CHECK_NE(integrator, nullptr);

	integrator->preprocess(*scene);
	integrator->render(*scene);

	//parsing_func("scenes/cornellBox/cornellBox.json");

	return 0;
}