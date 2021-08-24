#pragma once

#include "Rendering.h"
#include "Rtti.h"
#include "Primitive.h"
#include "../Math/KMathUtil.h"

RENDER_BEGIN

//! @brief Basic unit of objects in world.
	/**
	 * An entity is the very basic concept in a world. Everything, including camera, mesh, light or anything else is an
	 * entity. An entity could parse itself and decouple itself into one or multiple primitives depending its complexity.
	 * An entity itself doesn't touch rendering directly. It serves as a place where the logic operations should be performed.
	 */
class Entity : public AObject
{
public:
	typedef std::shared_ptr<Entity> ptr;

	Entity() = default;
	Entity(const APropertyTreeNode& node);

	Material* getMaterial() const { return m_material.get(); }
	const std::vector<Hitable::ptr>& getHitables() const { return m_hitables; }

	virtual std::string toString() const override { return "Entity[]"; }
	virtual ClassType getClassType() const override { return ClassType::RHitable; }

protected:
	Material::ptr m_material;
	std::vector<Hitable::ptr> m_hitables;
	Transform m_objectToWorld, m_worldToObject;
};

RENDER_END