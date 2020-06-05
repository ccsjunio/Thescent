#include "ColliderPlane.h"
#include <MathHelper.h>
#include <StringHelpers.h>
#include "Convert.h"
#include <BellEngineID.h>

ColliderPlane::ColliderPlane()
{	
	this->parent = nullptr;
	properties = new Properties();
	transformedProperties = new Properties();
}
ColliderPlane::ColliderPlane(const std::vector<Vector3D>& vertices)
{
	this->parent = nullptr;
	properties = new Properties(vertices);
	((Properties*)properties)->vertices = vertices;
	((Properties*)properties)->GenerateNormal();
	((Properties*)properties)->GenerateCenter();
	((Properties*)properties)->GenerateExtremes();
	
	transformedProperties = new Properties(vertices);
}
ColliderPlane::ColliderPlane(const std::vector<Vector3D>& vertices, const Matrix4D& worldMatrix)
{
	this->parent = nullptr;

	properties = new Properties(vertices);
	((Properties*)properties)->vertices = vertices;
	((Properties*)properties)->GenerateNormal();
	((Properties*)properties)->GenerateCenter();
	((Properties*)properties)->GenerateExtremes();

	transformedProperties = new Properties(vertices, worldMatrix);
}
ColliderPlane::ColliderPlane(Vector3D* vertices)
{
	this->parent = nullptr;

	properties = new Properties();
	((Properties*)properties)->vertices[0] = vertices[0];
	((Properties*)properties)->vertices[1] = vertices[1];
	((Properties*)properties)->vertices[2] = vertices[2];
	((Properties*)properties)->vertices[3] = vertices[3];
	((Properties*)properties)->GenerateNormal();
	((Properties*)properties)->GenerateCenter();
	((Properties*)properties)->GenerateExtremes();
	
	transformedProperties = new Properties();
}
ColliderPlane::~ColliderPlane()
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

void ColliderPlane::SetParent(void* parent)
{
	this->parent = parent;
}

void* ColliderPlane::GetParent()
{
	return this->parent;
}

const bool ColliderPlane::Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength)
{
	return false;
}

const bool ColliderPlane::Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength, ParameterContainer& output)
{
	return false;
}

const bool ColliderPlane::GetContent(const std::string& name, ParameterContainer& data)
{
	if (name == "normal")
	{
		data.Add("Vector3D", "normal", &((Properties*)transformedProperties)->normal);

		return true;
	}
	else if (name == "vertices")
	{
		data.Add("std::vector<Vector3D>", "vertices", &((ColliderPlane::Properties*)transformedProperties)->vertices);
		data.Add("size_t", "verticesCount", (void*)((ColliderPlane::Properties*)transformedProperties)->vertices.size());

		return true;
	}
	else if (name == "extremes")
	{
		data.Add("std::vector<Vector3D>", "extremes", &((ColliderPlane::Properties*)transformedProperties)->extremes);
		data.Add("size_t", "extremesCount", (void*)((ColliderPlane::Properties*)transformedProperties)->extremes.size());

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

void ColliderPlane::Transform(const Matrix4D& worldMatrix)
{
	transformedProperties->Transform(worldMatrix, properties);
}

const std::string ColliderPlane::GetName()
{
	return this->name;
}

void ColliderPlane::SetName(const std::string& name)
{
	this->name = name;
}

const std::string ColliderPlane::GetType()
{
	return COLLIDER_PLANE;
}

const unsigned long long ColliderPlane::GetTypeID()
{
	return ENGINE_OBJ_ID_PHY_COLLIDERPLANE;
}

ICloneable* ColliderPlane::Clone()
{
	ColliderPlane* collider = new ColliderPlane();
	collider->properties = (Properties*)properties->Clone();	
	collider->transformedProperties = (Properties*)transformedProperties->Clone();
	
	return collider;
}

ICollider::IProperties* ColliderPlane::GetProperties()
{
	return properties;
}

ICollider::IProperties* ColliderPlane::GetTransformedProperties()
{
	return transformedProperties;
}

btCollisionShape* ColliderPlane::GetWrappedShape()
{
	return ((ColliderPlane::Properties*)this->transformedProperties)->shape;
}