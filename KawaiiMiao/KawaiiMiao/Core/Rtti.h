#pragma once

#include "Rendering.h"
#include "../Math/KMathUtil.h"
#include "../Math/Transform.h"

#include <map>
#include <array>
#include <sstream>
#include <functional>

RENDER_BEGIN

/**
* \brief This is an associative container used to supply the constructors
* of \ref AObject subclasses with parameter information.
*/
class APropertyList final
{
public:
	APropertyList() = default;

	void set(const std::string& name, const std::string& value);
	void set(const std::string& name, const std::vector<std::string>& values);

	bool has(const std::string& name) const;

	bool getBoolean(const std::string& name) const;
	bool getBoolean(const std::string& name, const bool& defaultValue) const;
	Float getFloat(const std::string& name) const;
	Float getFloat(const std::string& name, const Float& defaultValue) const;
	int getInteger(const std::string& name) const;
	int getInteger(const std::string& name, const int& defaultValue) const;
	std::string getString(const std::string& name) const;
	std::string getString(const std::string& name, const std::string& defaultValue) const;
	Vector2f getVector2f(const std::string& name) const;
	Vector2f getVector2f(const std::string& name, const Vector2f& defaultValue) const;
	Vector3f getVector3f(const std::string& name) const;
	Vector3f getVector3f(const std::string& name, const Vector3f& defaultValue) const;
	std::vector<Float> getVectorNf(const std::string& name) const;
	std::vector<Float> getVectorNf(const std::string& name, const std::vector<Float>& defaultValue) const;

private:

	/* Custom variant data type */
	class AProperty final
	{
	public:

		AProperty() : values({}) { }
		bool empty() const { return values.empty(); }
		size_t size() const { return values.size(); }
		void addValue(const std::string& value) { values.push_back(value); }
		void setValue(const std::vector<std::string>& value_list) { values = value_list; }
		std::string& operator[](const size_t& index) { CHECK_LT(index, values.size()); return values[index]; }
		const std::string& operator[](const size_t& index) const { CHECK_LT(index, values.size()); return values[index]; }

	private:
		std::vector<std::string> values;
	};

	std::map<std::string, AProperty> m_properties;

	template<typename Type>
	Type get(const std::string& name, const size_t& index) const
	{
		auto it = m_properties.find(name);
		if (it == m_properties.end())
			K_INFO("Property {0} is missing!", name);

		//Note: only support float, int, bool and string
		auto valueStr = it->second[index];
		std::istringstream iss(valueStr);
		Type value;
		iss >> value;
		return  value;
	}

	template<>
	bool get<bool>(const std::string& name, const size_t& index) const
	{
		auto it = m_properties.find(name);
		if (it == m_properties.end())
			K_ERROR("Property is missing with {0} name : ", name);

		bool value = it->second[index] == "true" ? true : false;
		return value;
	}

	template<typename Type>
	Type get(const std::string& name, const size_t& index, const Type& defaulValue) const
	{
		auto it = m_properties.find(name);
		if (it == m_properties.end())
			return defaulValue;
		//Note: only support float, int, bool and string
		auto valueStr = it->second[index];
		std::istringstream iss(valueStr);
		Type value;
		iss >> value;
		return  value;
	}

	template<>
	bool get<bool>(const std::string& name, const size_t& index, const bool& defaulValue) const
	{
		auto it = m_properties.find(name);
		if (it == m_properties.end())
			return defaulValue;
		bool value = it->second[index] == "true" ? true : false;
		return value;
	}

	template<size_t N>
	std::array<Float, N> getVector(const AProperty& prop) const
	{
		std::array<Float, N> values;
		auto get_func = [&](const size_t& index) -> Float
		{
			auto valueStr = prop[index];
			std::istringstream iss(valueStr);
			Float value;
			iss >> value;
			return  value;
		};

		for (size_t i = 0; i < N; ++i)
			values[i] = get_func(i);

		return values;
	}

};

class APropertyTreeNode final
{
public:

	APropertyTreeNode(const std::string& nodeName) : m_nodeName(nodeName) {}

	std::string getTypeName() const;
	const APropertyList& getPropertyList() const;
	const std::string& getNodeName() const { return m_nodeName; }
	const APropertyTreeNode& getPropertyChild(const std::string& name) const;

	bool hasProperty(const std::string& name) const;
	bool hasPropertyChild(const std::string& name) const;

	void addProperty(const std::string& name, const std::string& value);
	void addProperty(const std::string& name, const std::vector<std::string>& values);
	void addChild(const APropertyTreeNode& child);

	static std::string m_directory;

private:
	std::string m_nodeName;
	APropertyList m_property;
	std::vector<APropertyTreeNode> m_children;
};


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


	/**
	 * \brief Add a child object to the current instance
	 *
	 * The default implementation does not support children and
	 * simply throws an exception
	 */
	virtual void addChild(AObject* child);

	/**
	 * \brief Set the parent object
	 *
	 * Subclasses may choose to override this method to be
	 * notified when they are added to a parent object. The
	 * default implementation does nothing.
	 */
	virtual void setParent(AObject* parent);

	/**
	 * \brief Perform some action associated with the object
	 *
	 * The default implementation throws an exception. Certain objects
	 * may choose to override it, e.g. to implement initialization,
	 * testing, or rendering functionality.
	 *
	 * This function is called by the XML parser once it has
	 * constructed an object and added all of its children
	 * using \ref addChild().
	 */

	virtual void activate();

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

/**
* \brief Factory for AObjects
*
* This utility class is part of a mini-RTTI framework and can
* instantiate arbitrary AObjects by their name.
*/

class AObjectFactory
{
public:
	typedef std::function<AObject* (const APropertyTreeNode&)> Constructor;

	static void registerClass(const std::string& type, const Constructor& constr);

	static AObject* createInstance(const std::string& type, const APropertyTreeNode& node);

private:

	static std::map<std::string, Constructor>& getConstrMap();
};

// Macro for registering an object constructor with the \ref AObjectFactory
#define RENDER_REGISTER_CLASS(cls, name) \
    inline cls *cls ##_create(const APropertyTreeNode &node) { \
        return new cls(node); \
    } \
    class cls ##_{ \
	public:\
        cls ##_() { \
            AObjectFactory::registerClass(name, cls ##_create); \
        } \
    };\
	static cls ##_ cls ##__RENDER_;

RENDER_END