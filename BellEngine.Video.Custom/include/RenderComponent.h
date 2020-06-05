#pragma once
#include <MathLibrary.h>
#include <IRender.h>
#include <ShaderFeature.h>
#include <vector>
#include <IDisposable.h>

class RenderComponent : public IRender
{
private:
	bool isActive;
	bool isVisible;
	int renderMode, alternateRenderMode;
	Vector4D baseDiffuseColor, baseSpecularColor;
	Vector4D currentDiffuseColor, currentSpecularColor;
	bool isBaseDiffuseSet, isBaseSpecularSet;
	std::string name;

public:
	std::vector<ShaderFeature*> shaderFeatures;

	RenderComponent();
	~RenderComponent();

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
	const std::string GetName();
	void SetName(const std::string& name);
	const std::string GetType();
	const unsigned long long GetTypeID();
	ICloneable* Clone();
	void Dispose();
};

