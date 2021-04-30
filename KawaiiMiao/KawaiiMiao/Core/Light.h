#pragma once

#include "Rendering.h"

RENDER_BEGIN

class Light
{
public:

};

class AreaLight : public Light
{
public:
	typedef std::shared_ptr<AreaLight> ptr;
};

RENDER_END