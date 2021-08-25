#pragma once

#include "../Core/Shape.h"

RENDER_BEGIN

class TriangleMesh final
{
public:
	typedef std::shared_ptr<TriangleMesh> ptr;
	typedef std::unique_ptr<TriangleMesh> unique_ptr;

	TriangleMesh(Transform* objectToWorld, const std::string& filename);

	size_t numTriangles() const { return m_indices.size() / 3; }
	size_t numVertices() const { return m_nVertices; }

	bool hasUV() const { return m_uv != nullptr; }
	bool hasNormal() const { return m_normal != nullptr; }

	const Vector3f& getPosition(const int& index) const { return m_position[index]; }
	const Vector3f& getNormal(const int& index) const { return m_normal[index]; }
	const Vector2f& getUV(const int& index) const { return m_uv[index]; }

	const std::vector<int>& getIndices() const { return m_indices; }

private:

	// TriangleMesh Data
	std::unique_ptr<Vector3f[]> m_position = nullptr;
	std::unique_ptr<Vector3f[]> m_normal = nullptr;
	std::unique_ptr<Vector2f[]> m_uv = nullptr;
	std::vector<int> m_indices;
	int m_nVertices;
};

class TriangleShape final : public Shape
{
public:
	typedef std::shared_ptr<TriangleShape> ptr;

	TriangleShape(const APropertyTreeNode& node);
	TriangleShape(Transform* objectToWorld, Transform* worldToObject,
		std::array<int, 3> indices, TriangleMesh* mesh);

	virtual ~TriangleShape() = default;

	virtual Float area() const override;

	virtual Interaction sample(const Vector2f& u, Float& pdf) const override;

	virtual Bounds3f objectBound() const override;
	virtual Bounds3f worldBound() const override;

	virtual bool hit(const Ray& ray) const override;
	virtual bool hit(const Ray& ray, Float& tHit, SurfaceInteraction& isect) const override;

	virtual Float solidAngle(const Vector3f& p, int nSamples = 512) const override;

	virtual std::string toString() const override { return "TriangleShape[]"; }

private:
	TriangleMesh* m_mesh;
	std::array<int, 3> m_indices;
};

RENDER_END