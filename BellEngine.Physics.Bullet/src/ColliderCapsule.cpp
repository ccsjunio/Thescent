#include "ColliderCapsule.h"
#include <BellEngineID.h>

const std::string ColliderCapsule::GetName()
{
	return this->name;
}

void ColliderCapsule::SetName(const std::string& name)
{
	this->name = name;
}

const std::string ColliderCapsule::GetType()
{
	return COLLIDER_CAPSULE;
}

const unsigned long long ColliderCapsule::GetTypeID()
{
	return ENGINE_OBJ_ID_PHY_COLLIDERCAPSULE;
}

ICloneable* ColliderCapsule::Clone()
{
	ColliderCapsule* collider = new ColliderCapsule();

	return collider;
}

ColliderCapsule::ColliderCapsule()
{
	this->parent = nullptr;
	properties = nullptr;
	transformedProperties = nullptr;
}

ColliderCapsule::~ColliderCapsule()
{
	if (properties)
	{
		delete (properties);
		properties = nullptr;
	}

	if (transformedProperties)
	{
		delete (transformedProperties);
		transformedProperties = nullptr;
	}
}

void ColliderCapsule::SetParent(void* parent)
{
	this->parent = parent;
}

void* ColliderCapsule::GetParent()
{
	return this->parent;
}

const bool ColliderCapsule::Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength)
{
	return false;
}

const bool ColliderCapsule::Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength, ParameterContainer& output)
{
	return false;
}

const bool ColliderCapsule::GetContent(const std::string& name, ParameterContainer& data)
{
	return false;
}

void ColliderCapsule::Transform(const Matrix4D& worldMatrix)
{
}

ICollider::IProperties* ColliderCapsule::GetProperties()
{
	return properties;
}

ICollider::IProperties* ColliderCapsule::GetTransformedProperties()
{
	return transformedProperties;
}