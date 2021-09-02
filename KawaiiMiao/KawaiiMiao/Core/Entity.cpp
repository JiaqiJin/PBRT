#include "Entity.h"
#include "../Core/Material.h"
#include "../Core/Light.h"
#include "../Core/Shape.h"

RENDER_BEGIN

RENDER_REGISTER_CLASS(Entity, "Entity");

Entity::Entity(const APropertyTreeNode& node)
{
	const APropertyList& props = node.getPropertyList();

	// Shape
	const auto& shapeNode = node.getPropertyChild("Shape");
	Shape::ptr shape = Shape::ptr(static_cast<Shape*>(AObjectFactory::createInstance(
		shapeNode.getTypeName(), shapeNode)));
	shape->setTransform(&m_objectToWorld, &m_worldToObject);

	// Transform
	Transform objectToWrold;
	const auto& shapeProps = shapeNode.getPropertyList();
	if (shapeNode.hasProperty("Transform"))
	{
		std::vector<Transform> transformStack;
		std::vector<Float> sequence = shapeProps.getVectorNf("Transform");
		size_t it = 0;
		bool undefined = false;
		while (it < sequence.size() && !undefined)
		{
			int token = static_cast<int>(sequence[it]);
			switch (token)
			{
			case 0://translate
				CHECK_LT(it + 3, sequence.size());
				Vector3f _trans = Vector3f(sequence[it + 1], sequence[it + 2], sequence[it + 3]);
				transformStack.push_back(translate(_trans));
				it += 4;
				break;
			case 1://scale
				CHECK_LT(it + 3, sequence.size());
				Vector3f _scale = Vector3f(sequence[it + 1], sequence[it + 2], sequence[it + 3]);
				transformStack.push_back(scale(_scale.x, _scale.y, _scale.z));
				it += 4;
				break;
			case 2://rotate
				CHECK_LT(it + 4, sequence.size());
				Vector3f axis = Vector3f(sequence[it + 1], sequence[it + 2], sequence[it + 3]);
				transformStack.push_back(rotate(sequence[it + 4], axis));
				it += 5;
				break;
			default:
				undefined = true;
				K_ERROR("Undefined transform action");
				break;
			}
		}

		//Note: calculate the transform matrix in a first-in-last-out manner
		if (!undefined)
		{
			for (auto it = transformStack.rbegin(); it != transformStack.rend(); ++it)
			{
				objectToWrold = objectToWrold * (*it);
			}
		}
	}
	m_objectToWorld = objectToWrold;
	m_worldToObject = inverse(m_objectToWorld);

	// Material
	const auto& materialNode = node.getPropertyChild("Material");
	m_material = Material::ptr(static_cast<Material*>(AObjectFactory::createInstance(
		materialNode.getTypeName(), materialNode)));

	//Area light
	AreaLight::ptr areaLight = nullptr;
	if (node.hasPropertyChild("Light"))
	{
		const auto& lightNode = node.getPropertyChild("Light");
		areaLight = AreaLight::ptr(static_cast<AreaLight*>(AObjectFactory::createInstance(
			lightNode.getTypeName(), lightNode)));
	}

	m_hitables.push_back(std::make_shared<HitableObject>(shape, m_material.get(), areaLight));
}

RENDER_END