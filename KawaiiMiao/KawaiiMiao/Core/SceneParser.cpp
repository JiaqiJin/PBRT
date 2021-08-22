#include "SceneParser.h"

#include <fstream>

#include "Film.h"
#include "Filter.h"
#include "Shape.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "../Accelerators/KDTree.h"

#include "../Tool/Logger.h"

using namespace nlohmann;

RENDER_BEGIN

void SceneParser::parser(const std::string& path, Scene::ptr& _scene, Integrator::ptr& _integrator)
{
	_integrator = nullptr;
	_scene = nullptr;

	json _scene_json;
	{
		std::ifstream infile(path);

		if (!infile)
		{
			K_ERROR("Could not open the json file: {0}", path);
		}

		infile >> _scene_json;
		infile.close();
	}

	K_INFO("Parse the scene file from {0}", path);

	auto get_func = [](const json_value_type& target) -> std::string
	{
		if (target.is_string())
		{
			return target.get<std::string>();
		}
		else if (target.is_boolean())
		{
			bool ret = target.get<bool>();
			return ret ? "true" : "false";
		}
		else if (target.is_number_float())
		{
			float ret = target.get<float>();
			std::stringstream ss;
			ss << ret;
			return ss.str();
		}
		else
		{
			int ret = target.get<int>();
			std::stringstream ss;
			ss << ret;
			return ss.str();
		}
	};

	//Setup directory path
	{
		size_t last_slash_idx = path.rfind('\\');
		if (last_slash_idx == std::string::npos)
		{
			last_slash_idx = path.rfind('/');
		}
		if (last_slash_idx != std::string::npos)
		{
			APropertyTreeNode::m_directory = path.substr(0, last_slash_idx + 1);
		}
	}
}

RENDER_END