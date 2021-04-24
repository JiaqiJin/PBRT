#include <iostream>
#include "Core/Rendering.h"
#include "Math/KMathUtil.h"

using namespace Render;

int main()
{
	Render::Vector2<float> a(1.0, 2.0);
	Render::Vector2<float> b(5.0, 3.0);
	
	Bounds2f bound(a, b);
	std::cout << bound.m_pMax.x << "  " << bound.m_pMax.y << bound.diagonal();

	return 0;
}