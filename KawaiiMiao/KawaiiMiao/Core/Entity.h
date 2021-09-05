#pragma once

#include "Rendering.h"
#include "Rtti.h"
#include "Primitive.h"
#include "../Shapes/TriangleShape.h"
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
	const std::vector<Primitive::ptr>& getPrimitives() const { return m_Primitives; }

	virtual std::string toString() const override { return "Entity[]"; }
	virtual ClassType getClassType() const override { return ClassType::RPrimitive; }

protected:
	Material::ptr m_material;
	std::vector<Primitive::ptr> m_Primitives;
	Transform m_objectToWorld, m_worldToObject;
};

class MeshEntity : public Entity
{
public:
	typedef std::shared_ptr<MeshEntity> ptr;

	MeshEntity(const APropertyTreeNode& node);

	virtual std::string toString() const override { return "MeshEntity[]"; }

private:
	TriangleMesh::unique_ptr m_mesh;
};


RENDER_END