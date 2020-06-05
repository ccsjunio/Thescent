#include "ColliderSphere.h"
#include <MathHelper.h>
#include <StringHelpers.h>
#include <BellEngineID.h>

ColliderSphere::ColliderSphere()
{
	this->parent = nullptr;
	properties = new ColliderSphere::Properties();
	transformedProperties = new ColliderSphere::Properties();
}
ColliderSphere::ColliderSphere(float radius, Vector3D center)
{
	this->parent = nullptr;
	properties = new ColliderSphere::Properties(radius, center);
	transformedProperties = new ColliderSphere::Properties(radius, center);
}
ColliderSphere::~ColliderSphere()
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

void ColliderSphere::SetParent(void* parent)
{
	this->parent = parent;
}

void* ColliderSphere::GetParent()
{
	return this->parent;
}

const bool ColliderSphere::Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength)
{
	ParameterContainer output;
	return Raycast(rayStart, direction, rayLength, output);
}

const bool ColliderSphere::Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength, ParameterContainer& output)
{
	Vector3D center = GetTransformedProperties()->GetTransform()->GetPosition();
	float radius = ((Properties*)GetTransformedProperties())->radius;

	int rc = MathHelper::TestRaySphere(rayStart, direction, center, radius);
	
	return rc != 0;
}

const bool ColliderSphere::GetContent(const std::string& name, ParameterContainer& data)
{
	if (name == "radius")
	{
		data.Add("float", "radius", &((ColliderSphere::Properties*)transformedProperties)->radius);

		return true;
	}
	else if (name == "extremes")
	{
		data.Add("std::vector<Vector3D>", "extremes", &((ColliderSphere::Properties*)transformedProperties)->extremes);
		data.Add("size_t", "extremesCount", (void*)((ColliderSphere::Properties*)transformedProperties)->extremes.size());

		return true;
	}
	else if (name == "center")
	{
		Vector3D center = transformedProperties->GetTransform()->GetPosition();
		data.Add("Vector3D", "center", &center);

		return true;
	}

	return false;
}

void ColliderSphere::Transform(const Matrix4D& worldMatrix)
{
	transformedProperties->Transform(worldMatrix, properties);
}

const std::string ColliderSphere::GetName()
{
	return this->name;
}

void ColliderSphere::SetName(const std::string& name)
{
	this->name = name;
}

const unsigned long long ColliderSphere::GetTypeID()
{
	return ENGINE_OBJ_ID_PHY_COLLIDERSPHERE;
}

const std::string ColliderSphere::GetType()
{
	return COLLIDER_SPHERE;
}

ICloneable* ColliderSphere::Clone()
{
	ColliderSphere* clone = new ColliderSphere();
	clone->properties = (ColliderSphere::Properties*)this->properties->Clone();
	clone->transformedProperties = (ColliderSphere::Properties*)this->transformedProperties->Clone();

	return clone;
}

ICollider::IProperties* ColliderSphere::GetProperties()
{
	return properties;
}

ICollider::IProperties* ColliderSphere::GetTransformedProperties()
{
	return transformedProperties;
}