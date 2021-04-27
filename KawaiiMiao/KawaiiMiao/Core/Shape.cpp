#include "Shape.h"

#include "Interaction.h"

#include <array>

RENDER_BEGIN

Shape::Shape(const Transform& objectToWorld, const Transform& worldToObject)
	: m_objectToWorld(objectToWorld), m_worldToObject(worldToObject)
{

}

bool Shape::hit(const Ray& ray) const
{
	Float tHit = ray.m_tMax;
	SurfaceInteraction isect;
	return hit(ray, tHit, isect);
}

Bounds3f Shape::worldBound() const { return m_objectToWorld(objectBound()); }

Interaction Shape::sample(const Interaction& ref, const Vector2f& u, Float& pdf) const
{
	// TODO
	Interaction it;
	it.p = Vector3f(0.0f);
	return it;
}

Float Shape::pdf(const Interaction& ref, const Vector3f& wi) const
{
	// TODO
	return 1.0f;
}

Float Shape::solidAngle(const Vector3f& p, int nSamples) const
{
	return 1.0f;
}

// Sphere

SphereShape::SphereShape(const Transform& objectToWorld, const Transform& worldToObject,
	const float radius) : Shape(objectToWorld, worldToObject), m_radius(radius) {}

Bounds3f SphereShape::objectBound() const
{
	return Bounds3f(Vector3f(-m_radius, -m_radius, -m_radius),
		Vector3f(m_radius, m_radius, m_radius));
}

Float SphereShape::area() const { return 4.0f * Pi * m_radius * m_radius; }

Interaction SphereShape::sample(const Vector2f& u, Float& pdf) const
{
	//TODO
	Interaction it;
	it.p = Vector3f(0.0f);
	return it;
}

Interaction SphereShape::sample(const Interaction& ref, const Vector2f& u, Float& pdf) const
{
	//TODO
	Interaction it;
	it.p = Vector3f(0.0f);
	return it;
}

Float SphereShape::pdf(const Interaction& ref, const Vector3f& wi) const
{
	// TODO
	return 1.0f;
}

bool SphereShape::hit(const Ray& r) const
{
	Float phi;
	Vector3f pHit;

	// Transform Ray to object space
	Ray ray = (m_worldToObject)(r);

	Float a = dot(ray.direction(), ray.direction());
	Float b = dot(ray.origin(), ray.direction());
	Float c = dot(ray.origin(), ray.origin()) - m_radius * m_radius;

	// discriminant
	Float discriminant = b * b - a * c;
	if (discriminant <= 0)
		return false;

	Float discr_sqrt = glm::sqrt(discriminant);
	Float t0 = (-b - discr_sqrt) / a;
	Float t1 = (-b + discr_sqrt) / a;

	if (t0 > t1)
		std::swap(t0, t1);

	if (t0 > ray.m_tMax || t1 <= 0)
		return false;

	Float tShapeHit = t0;
	if (tShapeHit <= 0)
	{
		tShapeHit = t1;
		if (tShapeHit > ray.m_tMax)
			return false;
	}

	return true;
}

bool SphereShape::hit(const Ray& r, Float& tHit, SurfaceInteraction& isect) const
{
	Float phi;
	Vector3f pHit;

	// Transform Ray to object space
	Ray ray = (m_worldToObject)(r);

	Float a = dot(ray.direction(), ray.direction());
	Float b = dot(ray.origin(), ray.direction());
	Float c = dot(ray.origin(), ray.origin()) - m_radius * m_radius;

	// discriminant
	Float discriminant = b * b - a * c;
	if (discriminant <= 0)
		return false;

	Float discr_sqrt = glm::sqrt(discriminant);
	Float t0 = (-b - discr_sqrt) / a;
	Float t1 = (-b + discr_sqrt) / a;

	if (t0 > t1)
		std::swap(t0, t1);

	if (t0 > ray.m_tMax || t1 <= 0)
		return false;

	Float tShapeHit = t0;
	if (tShapeHit <= 0)
	{
		tShapeHit = t1;
		if (tShapeHit > ray.m_tMax)
			return false;
	}

	pHit = ray((Float)tShapeHit);

	// Refine sphere intersection point
	pHit *= m_radius / distance(pHit, Vector3f(0, 0, 0));
	if (pHit.x == 0 && pHit.y == 0)
		pHit.x = 1e-5f * m_radius;

	phi = std::atan2(pHit.y, pHit.x);

	if (phi < 0)
		phi += 2 * Pi;

	Float theta = std::acos(clamp(pHit.z / m_radius, -1, 1));

	Float u = phi / (Pi * 2);
	Float v = (theta + PiOver2) / Pi;

	// Compute sphere $\dpdu$ and $\dpdv$
	Float zRadius = glm::sqrt(pHit.x * pHit.x + pHit.y * pHit.y);
	Float invZRadius = 1 / zRadius;
	Float cosPhi = pHit.x * invZRadius;
	Float sinPhi = pHit.y * invZRadius;
	Vector3f dpdu(-2 * Pi * pHit.y, 2 * Pi * pHit.x, 0);
	Vector3f dpdv = 2 * Pi * Vector3f(pHit.z * cosPhi, pHit.z * sinPhi, -m_radius * glm::sin(theta));

	isect = (m_objectToWorld)(SurfaceInteraction(pHit, Vector2f(u, v), -ray.direction(),
		dpdu, dpdv, this));

	isect.normal = faceforward(isect.normal, isect.wo);

	tHit = tShapeHit;

	return true;
}

Float SphereShape::solidAngle(const Vector3f& p, int nSamples) const
{
	Vector3f pCenter = (m_objectToWorld)(Vector3f(0, 0, 0), 1.0f);
	if (distanceSquared(p, pCenter) <= m_radius * m_radius)
		return 4 * Pi;
	Float sinTheta2 = m_radius * m_radius / distanceSquared(p, pCenter);
	Float cosTheta = glm::sqrt(glm::max((Float)0, 1 - sinTheta2));
	return (2 * Pi * (1 - cosTheta));
}

// Triangle

TriangleShape::TriangleShape(const Transform& objectToWorld, const Transform& worldToObject,
	Vector3f v[3]) : Shape(objectToWorld, worldToObject), m_p0(v[0]), m_p1(v[1]), m_p2(v[2])
{

}

Bounds3f TriangleShape::objectBound() const
{
	// Get triangle vertices in _p0_, _p1_, and _p2_
	return unionBounds(Bounds3f((m_worldToObject)(m_p0, 1.0f), (m_worldToObject)(m_p1, 1.0f)),
		(m_worldToObject)(m_p2, 1.0f));
}

Bounds3f TriangleShape::worldBound() const
{
	const Vector3f& p0 = m_p0;
	const Vector3f& p1 = m_p1;
	const Vector3f& p2 = m_p2;
	return unionBounds(Bounds3f(p0, p1), p2);
}

Float TriangleShape::area() const
{
	// Get triangle vertices in _p0_, _p1_, and _p2_
	const Vector3f& p0 = m_p0;
	const Vector3f& p1 = m_p1;
	const Vector3f& p2 = m_p2;
	return 0.5 * length(cross(p1 - p0, p2 - p0));
}

Interaction TriangleShape::sample(const Vector2f& u, Float& pdf) const
{
	// TODO
	Interaction it;
	it.p = Vector3f(0.0f);
	return it;
}

bool TriangleShape::hit(const Ray& ray) const
{
	// Get triangle vertices in _p0_, _p1_, and _p2_
	const Vector3f& p0 = m_p0;
	const Vector3f& p1 = m_p1;
	const Vector3f& p2 = m_p2;

	// Perform ray--triangle intersection test

	// Transform triangle vertices to ray coordinate space

	// Translate vertices based on ray origin
	Vector3f p0t = p0 - Vector3f(ray.origin());
	Vector3f p1t = p1 - Vector3f(ray.origin());
	Vector3f p2t = p2 - Vector3f(ray.origin());

	// Permute components of triangle vertices and ray direction
	int kz = maxDimension(abs(ray.direction()));
	int kx = kz + 1;
	if (kx == 3) kx = 0;
	int ky = kx + 1;
	if (ky == 3) ky = 0;
	Vector3f d = permute(ray.direction(), kx, ky, kz);
	p0t = permute(p0t, kx, ky, kz);
	p1t = permute(p1t, kx, ky, kz);
	p2t = permute(p2t, kx, ky, kz);

	// Apply shear transformation to translated vertex positions
	Float Sx = -d.x / d.z;
	Float Sy = -d.y / d.z;
	Float Sz = 1.f / d.z;
	p0t.x += Sx * p0t.z;
	p0t.y += Sy * p0t.z;
	p1t.x += Sx * p1t.z;
	p1t.y += Sy * p1t.z;
	p2t.x += Sx * p2t.z;
	p2t.y += Sy * p2t.z;

	// Compute edge function coefficients _e0_, _e1_, and _e2_
	Float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
	Float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
	Float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

	// Fall back to double precision test at triangle edges
	if (sizeof(Float) == sizeof(float) &&
		(e0 == 0.0f || e1 == 0.0f || e2 == 0.0f))
	{
		double p2txp1ty = (double)p2t.x * (double)p1t.y;
		double p2typ1tx = (double)p2t.y * (double)p1t.x;
		e0 = (float)(p2typ1tx - p2txp1ty);
		double p0txp2ty = (double)p0t.x * (double)p2t.y;
		double p0typ2tx = (double)p0t.y * (double)p2t.x;
		e1 = (float)(p0typ2tx - p0txp2ty);
		double p1txp0ty = (double)p1t.x * (double)p0t.y;
		double p1typ0tx = (double)p1t.y * (double)p0t.x;
		e2 = (float)(p1typ0tx - p1txp0ty);
	}

	// Perform triangle edge and determinant tests
	if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
		return false;
	Float det = e0 + e1 + e2;
	if (det == 0)
		return false;

	// Compute scaled hit distance to triangle and test against ray $t$ range
	p0t.z *= Sz;
	p1t.z *= Sz;
	p2t.z *= Sz;
	Float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
	if (det < 0 && (tScaled >= 0 || tScaled < ray.m_tMax * det))
		return false;
	else if (det > 0 && (tScaled <= 0 || tScaled > ray.m_tMax * det))
		return false;

	// Compute barycentric coordinates and $t$ value for triangle intersection
	Float invDet = 1 / det;
	Float b0 = e0 * invDet;
	Float b1 = e1 * invDet;
	Float b2 = e2 * invDet;
	Float t = tScaled * invDet;

	// Ensure that computed triangle $t$ is conservatively greater than zero

	// Compute $\delta_z$ term for triangle $t$ error bounds
	Float maxZt = maxComponent(abs(Vector3f(p0t.z, p1t.z, p2t.z)));
	Float deltaZ = gamma(3) * maxZt;

	// Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
	Float maxXt = maxComponent(abs(Vector3f(p0t.x, p1t.x, p2t.x)));
	Float maxYt = maxComponent(abs(Vector3f(p0t.y, p1t.y, p2t.y)));
	Float deltaX = gamma(5) * (maxXt + maxZt);
	Float deltaY = gamma(5) * (maxYt + maxZt);

	// Compute $\delta_e$ term for triangle $t$ error bounds
	Float deltaE = 2 * (gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

	// Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
	Float maxE = maxComponent(abs(Vector3f(e0, e1, e2)));
	Float deltaT = 3 * (gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) * glm::abs(invDet);
	if (t <= deltaT)
		return false;

	return true;
}

bool TriangleShape::hit(const Ray& ray, Float& tHit, SurfaceInteraction& isect) const
{
	// Get triangle vertices in _p0_, _p1_, and _p2_
	const Vector3f& p0 = m_p0;
	const Vector3f& p1 = m_p1;
	const Vector3f& p2 = m_p2;

	// Perform ray--triangle intersection test

	// Transform triangle vertices to ray coordinate space

	// Translate vertices based on ray origin
	Vector3f p0t = p0 - Vector3f(ray.origin());
	Vector3f p1t = p1 - Vector3f(ray.origin());
	Vector3f p2t = p2 - Vector3f(ray.origin());

	// Permute components of triangle vertices and ray direction
	int kz = maxDimension(abs(ray.direction()));
	int kx = kz + 1;
	if (kx == 3) kx = 0;
	int ky = kx + 1;
	if (ky == 3) ky = 0;
	Vector3f d = permute(ray.direction(), kx, ky, kz);
	p0t = permute(p0t, kx, ky, kz);
	p1t = permute(p1t, kx, ky, kz);
	p2t = permute(p2t, kx, ky, kz);

	// Apply shear transformation to translated vertex positions
	Float Sx = -d.x / d.z;
	Float Sy = -d.y / d.z;
	Float Sz = 1.f / d.z;
	p0t.x += Sx * p0t.z;
	p0t.y += Sy * p0t.z;
	p1t.x += Sx * p1t.z;
	p1t.y += Sy * p1t.z;
	p2t.x += Sx * p2t.z;
	p2t.y += Sy * p2t.z;

	// Compute edge function coefficients _e0_, _e1_, and _e2_
	Float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
	Float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
	Float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

	// Fall back to double precision test at triangle edges
	if (sizeof(Float) == sizeof(float) &&
		(e0 == 0.0f || e1 == 0.0f || e2 == 0.0f))
	{
		double p2txp1ty = (double)p2t.x * (double)p1t.y;
		double p2typ1tx = (double)p2t.y * (double)p1t.x;
		e0 = (float)(p2typ1tx - p2txp1ty);
		double p0txp2ty = (double)p0t.x * (double)p2t.y;
		double p0typ2tx = (double)p0t.y * (double)p2t.x;
		e1 = (float)(p0typ2tx - p0txp2ty);
		double p1txp0ty = (double)p1t.x * (double)p0t.y;
		double p1typ0tx = (double)p1t.y * (double)p0t.x;
		e2 = (float)(p1typ0tx - p1txp0ty);
	}

	// Perform triangle edge and determinant tests
	if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
		return false;
	Float det = e0 + e1 + e2;
	if (det == 0)
		return false;

	// Compute scaled hit distance to triangle and test against ray $t$ range
	p0t.z *= Sz;
	p1t.z *= Sz;
	p2t.z *= Sz;
	Float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
	if (det < 0 && (tScaled >= 0 || tScaled < ray.m_tMax * det))
		return false;
	else if (det > 0 && (tScaled <= 0 || tScaled > ray.m_tMax * det))
		return false;

	// Compute barycentric coordinates and $t$ value for triangle intersection
	Float invDet = 1 / det;
	Float b0 = e0 * invDet;
	Float b1 = e1 * invDet;
	Float b2 = e2 * invDet;
	Float t = tScaled * invDet;

	// Ensure that computed triangle $t$ is conservatively greater than zero

	// Compute $\delta_z$ term for triangle $t$ error bounds
	Float maxZt = maxComponent(abs(Vector3f(p0t.z, p1t.z, p2t.z)));
	Float deltaZ = gamma(3) * maxZt;

	// Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
	Float maxXt = maxComponent(abs(Vector3f(p0t.x, p1t.x, p2t.x)));
	Float maxYt = maxComponent(abs(Vector3f(p0t.y, p1t.y, p2t.y)));
	Float deltaX = gamma(5) * (maxXt + maxZt);
	Float deltaY = gamma(5) * (maxYt + maxZt);

	// Compute $\delta_e$ term for triangle $t$ error bounds
	Float deltaE = 2 * (gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

	// Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
	Float maxE = maxComponent(abs(Vector3f(e0, e1, e2)));
	Float deltaT = 3 * (gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) * glm::abs(invDet);
	if (t <= deltaT)
		return false;

	// Compute triangle partial derivatives
	Vector3f dpdu, dpdv;
	Vector2f uv[3] = { Vector2f(0,0), Vector2f(1,0), Vector2f(1,1) };
	//GetUVs(uv);

	// Compute deltas for triangle partial derivatives
	Vector2f duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
	Vector3f dp02 = p0 - p2, dp12 = p1 - p2;
	Float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
	bool degenerateUV = glm::abs(determinant) < 1e-8;
	if (!degenerateUV)
	{
		Float invdet = 1 / determinant;
		dpdu = (duv12[1] * dp02 - duv02[1] * dp12) * invdet;
		dpdv = (-duv12[0] * dp02 + duv02[0] * dp12) * invdet;
	}
	if (degenerateUV || lengthSquared(cross(dpdu, dpdv)) == 0)
	{
		// Handle zero determinant for triangle partial derivative matrix
		Vector3f ng = cross(p2 - p0, p1 - p0);
		// The triangle is actually degenerate; the intersection is bogus.
		if (lengthSquared(ng) == 0)
			return false;

		coordinateSystem(normalize(ng), dpdu, dpdv);
	}

	// Interpolate $(u,v)$ parametric coordinates and hit point
	Vector3f pHit = b0 * p0 + b1 * p1 + b2 * p2;
	Vector2f uvHit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];

	// Fill in _SurfaceInteraction_ from triangle hit
	isect = SurfaceInteraction(pHit, uvHit, -ray.direction(), dpdu, dpdv, this);

	// Override surface normal in _isect_ for triangle
	isect.normal = Vector3f(normalize(cross(dp02, dp12)));
	tHit = t;

	return true;
}

Float TriangleShape::solidAngle(const Vector3f& p, int nSamples) const
{
	// Project the vertices into the unit sphere around p.
	std::array<Vector3f, 3> pSphere = { normalize(m_p0 - p), normalize(m_p1 - p), normalize(m_p2 - p) };

	// http://math.stackexchange.com/questions/9819/area-of-a-spherical-triangle
	// Girard's theorem: surface area of a spherical triangle on a unit
	// sphere is the 'excess angle' alpha+beta+gamma-pi, where
	// alpha/beta/gamma are the interior angles at the vertices.
	//
	// Given three vertices on the sphere, a, b, c, then we can compute,
	// for example, the angle c->a->b by
	//
	// cos theta =  Dot(Cross(c, a), Cross(b, a)) /
	//              (Length(Cross(c, a)) * Length(Cross(b, a))).
	//
	Vector3f cross01 = (cross(pSphere[0], pSphere[1]));
	Vector3f cross12 = (cross(pSphere[1], pSphere[2]));
	Vector3f cross20 = (cross(pSphere[2], pSphere[0]));

	// Some of these vectors may be degenerate. In this case, we don't want
	// to normalize them so that we don't hit an assert. This is fine,
	// since the corresponding dot products below will be zero.
	if (lengthSquared(cross01) > 0) cross01 = normalize(cross01);
	if (lengthSquared(cross12) > 0) cross12 = normalize(cross12);
	if (lengthSquared(cross20) > 0) cross20 = normalize(cross20);

	// We only need to do three cross products to evaluate the angles at
	// all three vertices, though, since we can take advantage of the fact
	// that Cross(a, b) = -Cross(b, a).
	return glm::abs(
		glm::acos(clamp(dot(cross01, -cross12), -1, 1)) +
		glm::acos(clamp(dot(cross12, -cross20), -1, 1)) +
		glm::acos(clamp(dot(cross20, -cross01), -1, 1)) - Pi);
}

RENDER_END