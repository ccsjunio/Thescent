#pragma once
#include "IComponent.h"
#include <MathLibrary.h>
#include "ShaderFeature.h"
#include <vector>

class IRender : public IComponent
{
protected:
	IRender() {};

public:
	virtual ~IRender() {};

	virtual const Vector4D GetBaseDiffuseColor()= 0;
	virtual const Vector4D GetBaseSpecularColor()= 0;
	virtual void SetDiffuse(const Vector4D& color)= 0;
	virtual const Vector4D GetDiffuse()= 0;
	virtual void SetSpecular(const Vector4D& color)= 0;
	virtual const Vector4D GetSpecular()= 0;
	virtual const bool IsVisible()= 0;
	virtual void SetVisibility(const bool& option)= 0;
	virtual void SetRenderMode(const int& renderMode)= 0;
	virtual const int GetRenderMode()= 0;
	virtual const bool GetShaderFeatures(std::vector<ShaderFeature*>& shaderFeatures)= 0;
	virtual const bool HasShaderFeature(const std::string& name) = 0;
	virtual ShaderFeature* GetShaderFeature(const std::string& name) = 0;
	virtual const bool Append(ShaderFeature* sf)= 0;
	virtual const bool Remove(ShaderFeature* sf)= 0;
};