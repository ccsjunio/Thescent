#include "ColliderCapsule.h"
#include <BellEngineID.h>

const std::string ColliderCapsule::GetType()
{
	return COLLIDER_CAPSULE;
}

ICloneable* ColliderCapsule::Clone()
{
	ColliderCapsule* clone = new ColliderCapsule();
	clone->SetName(GetName());
	clone->properties = (ColliderCapsule::Properties*)properties->Clone();
	clone->transformedProperties = (ColliderCapsule::Properties*)transformedProperties->Clone();
	
	return clone;
}

ColliderCapsule::ColliderCapsule(const Vector3D& pointA, const Vector3D& pointB, const float& radius)
{
	properties = new ColliderCapsule::Properties(radius, pointA, pointB);
	transformedProperties = new ColliderCapsule::Properties(radius, pointA, pointB);
}

ColliderCapsule::ColliderCapsule()
{
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
	ParameterContainer output;
	return Raycast(rayStart, direction, rayLength, output);
}

const bool ColliderCapsule::Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength, ParameterContainer& output)
{
	/*Vector3D center = GetTransformedProperties()->GetTransform()->GetPosition();
	float radius = ((Properties*)GetTransformedProperties())->radius;*/
	Vector3D rayEnd = rayStart + direction * rayLength;

	//int rc = MathHelper::TestRaySphere(rayStart, direction, center, radius);

	//float totalCapsuleLength = 2 * radius + glm::distance(((Properties*)GetTransformedProperties())->pointB, ((Properties*)GetTransformedProperties())->pointA);
	//Vector3D capsuleVector = ((Properties*)GetTransformedProperties())->pointB - ((Properties*)GetTransformedProperties())->pointA;
	//Vector3D capsuleVectorDirection = glm::normalize(capsuleVector);

	float t = 0.0f;
	int rc = MathHelper::IntersectRayCylinder(rayStart, rayEnd, ((Properties*)GetTransformedProperties())->pointA, ((Properties*)GetTransformedProperties())->pointB, ((Properties*)GetTransformedProperties())->radius, t);

	return rc != 0;
}

const bool ColliderCapsule::GetContent(const std::string& name, ParameterContainer& data)
{
	return false;
}

void ColliderCapsule::Transform(const Matrix4D& worldMatrix)
{
	this->transformedProperties->Transform(worldMatrix, properties);
}

const std::string ColliderCapsule::GetName()
{
	return this->name;
}

void ColliderCapsule::SetName(const std::string& name)
{
	this->name = name;
}

ICollider::IProperties* ColliderCapsule::GetProperties()
{
	return properties;
}

ICollider::IProperties* ColliderCapsule::GetTransformedProperties()
{
	return transformedProperties;
}

const unsigned long long ColliderCapsule::GetTypeID()
{
	return ENGINE_OBJ_ID_PHY_COLLIDERCAPSULE;
}