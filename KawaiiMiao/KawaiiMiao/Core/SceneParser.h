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
	static void parser(const std::string& path);

private:
	using json_value = nlohmann::basic_json<>::value_type;
};

RENDER_END