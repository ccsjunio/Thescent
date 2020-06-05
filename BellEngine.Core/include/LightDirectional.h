#pragma once
#include <ILight.h>

class LightDirectional : public ILight
{
private:
	bool isActive;
	bool isVisible;
	Vector4D baseDiffuseColor, baseSpecularColor;
	Vector4D currentDiffuseColor, currentSpecularColor;
	bool isBaseDiffuseSet, isBaseSpecularSet;
	std::string name;
	ICollider* collider;
	Vector3D direction;

public:
	
	LightDirectional();
	~LightDirectional();

	const bool IsActive();
	void Activate(const bool& option = true);
	void Dispose();

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

	void SetDirection(const Vector3D& direction);
	const Vector3D GetDirection();
	unsigned short GetEnumID();
	const std::string GetType();
	void Update(const double& deltaTime);
	const unsigned long long GetTypeID();
	Matrix4D GetWorldMatrix();
	Matrix4D GetTranslationMatrix();
	Matrix4D GetScaleMatrix();
	Matrix4D GetRotationMatrix();
	ICloneable* Clone();
	const std::string GetName();
	void SetName(const std::string& name);
};

