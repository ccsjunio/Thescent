#pragma once
#include <MathLibrary.h>
#include <ICollider.h>

#define COLLIDER_CAPSULE "Capsule"

class ColliderCapsule : public ICollider
{
protected:
	IProperties* properties;
	IProperties* transformedProperties;
	std::string name;
	void* parent;

public:
	ColliderCapsule();
	~ColliderCapsule();
		
	void SetParent(void* parent);
	void* GetParent();
	const bool Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength);
	const bool Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength, ParameterContainer& output);
	const bool GetContent(const std::string& name,  ParameterContainer& data);
	void Transform(const Matrix4D& worldMatrix);
	const std::string GetName();
	void SetName(const std::string& name);
	const std::string GetType();
	const unsigned long long GetTypeID();
	ICloneable* Clone();
	ICollider::IProperties* GetProperties();
	ICollider::IProperties* GetTransformedProperties();
};

