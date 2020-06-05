#include "ColliderParallelepiped.h"
#include <StringHelpers.h>
#include <BellEngineID.h>

ColliderBox::ColliderBox()
{
	this->parent = nullptr;

	properties = new Properties();
	((ColliderBox::Properties*)properties)->GenerateCenter();
	((ColliderBox::Properties*)properties)->GenerateVertices();
	properties->GenerateExtremes();

	transformedProperties = new Properties();
	((ColliderBox::Properties*)transformedProperties)->GenerateCenter();
	((ColliderBox::Properties*)transformedProperties)->GenerateVertices();
	transformedProperties->GenerateExtremes();
}
ColliderBox::ColliderBox(Vector3D max, Vector3D min)
{
	this->parent = nullptr;

	properties = new Properties(min, max);
	((ColliderBox::Properties*)properties)->GenerateVertices();
	((ColliderBox::Properties*)properties)->GenerateCenter();
	properties->GenerateExtremes();

	transformedProperties = new Properties(min, max);
	((ColliderBox::Properties*)transformedProperties)->GenerateVertices();
	((ColliderBox::Properties*)transformedProperties)->GenerateCenter();
	transformedProperties->GenerateExtremes();
}
ColliderBox::~ColliderBox()
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

void ColliderBox::SetParent(void* parent)
{
	this->parent = parent;
}

void* ColliderBox::GetParent()
{
	return this->parent;
}

const bool ColliderBox::Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength)
{
	return false;
}

const bool ColliderBox::Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength, ParameterContainer& output)
{
	return false;
}
const bool ColliderBox::GetContent(const std::string& name, ParameterContainer& data)
{
	if (name == "vertices")
	{
		data.Add("Vector3D[]", "vertices", &((ColliderBox::Properties*)transformedProperties)->vertices);
		data.Add("unsigned int", "verticesCount", (void*)ColliderBox::Properties::MAX_VERT_COUNT);

		return true;
	}
	else if (name == "extremes")
	{
		data.Add("std::vector<Vector3D>", "extremes", &((ColliderBox::Properties*)transformedProperties)->extremes);
		data.Add("unsigned int", "extremesCount", (void*)((ColliderBox::Properties*)transformedProperties)->extremes.size());

		return true;
	}
	else if (name == "min")
	{
		data.Add("Vector3D", "min", &((ColliderBox::Properties*)transformedProperties)->min);

		return true;
	}
	else if (name == "max")
	{
		data.Add("Vector3D", "max", &((ColliderBox::Properties*)transformedProperties)->max);

		return true;
	}

	return false;
}
void ColliderBox::Transform(const Matrix4D& worldMatrix)
{
	transformedProperties->Transform(worldMatrix, properties);
}

const std::string ColliderBox::GetName()
{
	return this->name;
}

void ColliderBox::SetName(const std::string& name)
{
	this->name = name;
}

const std::string ColliderBox::GetType()
{
	return COLLIDER_BOX;
}

const unsigned long long ColliderBox::GetTypeID()
{
	return ENGINE_OBJ_ID_PHY_COLLIDERBOX;
}

ICloneable* ColliderBox::Clone()
{
	ColliderBox* collider = new ColliderBox();
	collider->properties = (Properties*)this->properties->Clone();
	collider->transformedProperties = (Properties*)this->transformedProperties->Clone();
	
	for (unsigned int vert = 0; vert < 8; vert++)
	{
		((Properties*)collider->properties)->vertices[vert] = ((Properties*)this->properties)->vertices[vert];
		((Properties*)collider->transformedProperties)->vertices[vert] = ((Properties*)this->transformedProperties)->vertices[vert];
	}
	
	return collider;
}

ICollider::IProperties* ColliderBox::GetProperties()
{
	return properties;
}

ICollider::IProperties* ColliderBox::GetTransformedProperties()
{
	return transformedProperties;
}