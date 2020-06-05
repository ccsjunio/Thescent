#pragma once
#include <ShaderFeature.h>
#include <IClothRenderComponent.h>

class ClothRenderComponent : public IClothRenderComponent
{
private:
	bool isActive;
	bool isVisible;
	int renderMode, alternateRenderMode;
	Vector4D baseDiffuseColor, baseSpecularColor;
	Vector4D currentDiffuseColor, currentSpecularColor;
	bool isBaseDiffuseSet, isBaseSpecularSet;
	std::vector<ShaderFeature*> shaderFeatures;
	std::string name;

public:

	ClothRenderComponent();
	~ClothRenderComponent();

	const bool GetClothMDI(const size_t& rowCount, const size_t& columnCount, const std::vector<Vector3D>& nodePositions, ModelDrawInfo& mdi);
	const bool GetClothTriangles(const size_t& rowCount, const size_t& columnCount, const std::vector<Vector3D>& nodePositions, std::vector<Triangle3D>& triangles);
	const bool IsActive();
	void Activate(const bool& value = true);
	const Vector4D GetBaseDiffuseColor();
	const Vector4D GetBaseSpecularColor();
	void SetDiffuse(const Vector4D& color);
	const Vector4D GetDiffuse();
	void SetSpecular(const Vector4D& color);
	const Vector4D GetSpecular();
	const bool IsVisible();
	void SetVisibility(const bool& option);
	void SetRenderMode(const int& renderMode);
	const int GetRenderMode();
	void SetAlternateRenderMode(const int& renderMode);
	const int GetAlternateRenderMode();
	const bool GetShaderFeatures(std::vector<ShaderFeature*>& shaderFeatures);
	const bool HasShaderFeature(const std::string& name);
	ShaderFeature* GetShaderFeature(const std::string& name);
	const bool Append(ShaderFeature* sf);
	const bool Remove(ShaderFeature* sf);
	const std::string GetType();
	const unsigned long long GetTypeID();
	const std::string GetName();
	void SetName(const std::string& name);
	ICloneable* Clone();
	void Dispose();
};

