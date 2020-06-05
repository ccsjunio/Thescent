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
ColliderSphere::ColliderSphere(const float& radius, const Vector3D& center)
{
	this->parent = nullptr;
	properties = new ColliderSphere::Properties(radius, center);
	transformedProperties = new ColliderSphere::Properties(radius, center);	
}
ColliderSphere::ColliderSphere(const float& radius, const Vector3D& center, const Matrix4D& worldMatrix)
{
	this->parent = nullptr;
	properties = new ColliderSphere::Properties(radius, center);
	transformedProperties = new ColliderSphere::Properties(radius, center, worldMatrix);
}
ColliderSphere::~ColliderSphere()
{
	/*if (shape)
	{
		delete (shape);
		shape = nullptr;
	}*/

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
	return false;
}

const bool ColliderSphere::Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength, ParameterContainer& output)
{
	return false;
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

const std::string ColliderSphere::GetType()
{
	return COLLIDER_SPHERE;
}

const unsigned long long ColliderSphere::GetTypeID()
{
	return ENGINE_OBJ_ID_PHY_COLLIDERSPHERE;
}

ICloneable* ColliderSphere::Clone()
{
	ColliderSphere* collider = new ColliderSphere();
	collider->properties = (ColliderSphere::Properties*)collider->properties->Clone();
	collider->transformedProperties = (ColliderSphere::Properties*)collider->transformedProperties->Clone();

	return collider;
}

ICollider::IProperties* ColliderSphere::GetProperties()
{
	return properties;
}

ICollider::IProperties* ColliderSphere::GetTransformedProperties()
{
	return transformedProperties;
}

btCollisionShape* ColliderSphere::GetWrappedShape()
{
	return ((ColliderSphere::Properties*)this->transformedProperties)->shape;
}