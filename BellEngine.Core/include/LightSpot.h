#pragma once
#include <ILight.h>

class LightSpot : public ILight
{
private:
	bool isActive;
	bool isVisible;
	Vector4D baseDiffuseColor, baseSpecularColor;
	Vector4D currentDiffuseColor, currentSpecularColor;
	bool isBaseDiffuseSet, isBaseSpecularSet;
	std::string name;
	ICollider* collider;
	float constAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	float cutOffDistance;
	float innerAngle;
	float outerAngle;
	Vector3D direction;

public:

	

	LightSpot();
	~LightSpot();

	const bool IsActive();
	void Activate(const bool& option = true);
	void Dispose();

	void SetInnerAngle(const float& value);
	const float GetInnerAngle();
	void SetOuterAngle(const float& value);
	const float GetOuterAngle();

	void SetDirection(const Vector3D& direction);
	const Vector3D GetDirection();
	void SetConstantAttenuation(const float& value);
	const float GetConstantAttenuation();
	void SetLinearAttenuation(const float& value);
	const float GetLinearAttenuation();
	void SetQuadraticAttenuation(const float& value);
	const float GetQuadraticAttenuation();
	void SetCutoffDistance(const float& value);
	const float GetCutoffDistance();
	const Vector4D GetBaseDiffuseColor();
	const Vector4D GetBaseSpecularColor();
	void SetDiffuse(const Vector4D& color);
	const Vector4D GetDiffuse();
	void SetSpecular(const Vector4D& color);
	const Vector4D GetSpecular();
	const bool IsVisible();
	void SetVisibility(const bool& option);
	void SetCollider(ICollider* ptr);
	ICollider* GetCollider();

	const std::string GetName();
	void SetName(const std::string& name);
	unsigned short GetEnumID();
	const std::string GetType();
	void Update(const double& deltaTime);
	const unsigned long long GetTypeID();
	Matrix4D GetWorldMatrix();
	Matrix4D GetTranslationMatrix();
	Matrix4D GetScaleMatrix();
	Matrix4D GetRotationMatrix();

	ICloneable* Clone();
};

