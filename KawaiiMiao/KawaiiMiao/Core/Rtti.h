#pragma once

#include "Rendering.h"
#include "../Math/KMathUtil.h"
#include "../Math/Transform.h"

RENDER_BEGIN

class AObject
{
public:
	enum ClassType
	{
		RHitable = 0,
		RShape,
		RMaterial,
		RLight,
		RCamera,
		RIntegrator,
		RSampler,
		RFilter,
		RFilm,
		REntity,
		EClassTypeCount
	};

	virtual ~AObject() = default;

	virtual ClassType getClassType() const = 0;

	virtual std::string toString() const = 0;

	static std::string getClassTypeName(ClassType type)
	{
		switch (type)
		{
		case RMaterial:   return "Material";
		case RHitable:    return "Hitable";
		case RShape:	   return "Shape";
		case RLight:      return "Light";
		case RCamera:	   return "Camera";
		case RIntegrator: return "Integrator";
		case RSampler:	   return "Sampler";
		case RFilter:     return "Filter";
		case RFilm:       return "Film";
		case REntity:	   return "Entity";
		default:           return "Unknown";
		}
	}

};

RENDER_END