#pragma once

#include "Rendering.h"
#include "../Math/KMathUtil.h"
#include "Scene.h"
#include "Integrator.h"

#include <json/json.hpp>

RENDER_BEGIN

class SceneParser
{
public:
	static void parser(const std::string& path, Scene::ptr& _scene, Integrator::ptr& integrator);

private:
	using json_value_type = nlohmann::basic_json<>::value_type;
};

RENDER_END