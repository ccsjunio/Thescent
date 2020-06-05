#include "RenderComponent.h"
#include <BellEngineID.h>

RenderComponent::RenderComponent()
{
	isActive = true;
	baseSpecularColor = currentDiffuseColor = baseDiffuseColor = currentSpecularColor =Vector4D();
	isBaseDiffuseSet = isBaseSpecularSet = false;
	isVisible = true;
	renderMode = alternateRenderMode = 0;
};
RenderComponent::~RenderComponent() 
{
	Dispose();
};

const bool RenderComponent::IsActive()
{
	return isActive;
}
void RenderComponent::Activate(const bool& value)
{
	isActive = value;
}
const Vector4D RenderComponent::GetBaseDiffuseColor()
{
	return this->baseDiffuseColor;
}
const Vector4D RenderComponent::GetBaseSpecularColor()
{
	return this->baseSpecularColor;
}
void RenderComponent::SetDiffuse(const Vector4D& color)
{
	this->currentDiffuseColor = color;

	if (!isBaseDiffuseSet)
	{
		this->baseDiffuseColor = color;
		isBaseDiffuseSet = true;
	}
}
const Vector4D RenderComponent::GetDiffuse()
{
	return currentDiffuseColor;
}
void RenderComponent::SetSpecular(const Vector4D& color)
{
	this->currentSpecularColor = color;

	if (!isBaseSpecularSet)
	{
		this->baseSpecularColor = color;
		isBaseSpecularSet = true;
	}
}
const Vector4D RenderComponent::GetSpecular()
{
	return currentSpecularColor;
}
const bool RenderComponent::IsVisible()
{
	return isVisible;
}
void RenderComponent::SetVisibility(const bool& option)
{
	isVisible = option;
}
void RenderComponent::SetRenderMode(const int& renderMode)
{
	this->renderMode = renderMode;
}
const int RenderComponent::GetRenderMode()
{
	return renderMode;
}
void RenderComponent::SetAlternateRenderMode(const int& renderMode)
{
	this->alternateRenderMode = renderMode;
}
const int RenderComponent::GetAlternateRenderMode()
{
	return this->alternateRenderMode;
}
const bool RenderComponent::GetShaderFeatures(std::vector<ShaderFeature*>& shaderFeatures)
{
	shaderFeatures = this->shaderFeatures;
	return !shaderFeatures.empty();
}
const bool RenderComponent::HasShaderFeature(const std::string& name)
{
	for (ShaderFeature*& sf : shaderFeatures)
	{
		if (sf->GetName() == name)
		{
			return true;
		}
	}

	return false;
}
ShaderFeature* RenderComponent::GetShaderFeature(const std::string& name)
{
	for (ShaderFeature*& sf : shaderFeatures)
	{
		if (sf->GetName() == name)
		{
			return sf;
		}
	}

	return nullptr;
}
const bool RenderComponent::Append(ShaderFeature* sf)
{
	if (sf)
	{
		auto it = std::find(shaderFeatures.begin(), shaderFeatures.end(), sf);
		if (it == shaderFeatures.end())
		{
			shaderFeatures.push_back(sf);
			return true;
		}
	}

	return false;
}
const bool RenderComponent::Remove(ShaderFeature* sf)
{
	if (sf)
	{
		auto it = std::find(shaderFeatures.begin(), shaderFeatures.end(), sf);
		if (it != shaderFeatures.end())
		{
			shaderFeatures.erase(it);
			return true;
		}
	}

	return false;
}
const std::string RenderComponent::GetName()
{
	return this->name;
}
void RenderComponent::SetName(const std::string& name)
{
	this->name = name;
}
const std::string RenderComponent::GetType()
{
	return "RenderComponent";
}
const unsigned long long RenderComponent::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_RENDERCOMPONENT;
}
ICloneable* RenderComponent::Clone()
{
	RenderComponent* clone = new RenderComponent();

	clone->renderMode = renderMode;
	clone->SetVisibility(isVisible);
	clone->baseDiffuseColor = baseDiffuseColor;
	clone->baseSpecularColor = baseSpecularColor;
	clone->currentDiffuseColor = currentDiffuseColor;
	clone->currentSpecularColor = currentSpecularColor;
	clone->isBaseDiffuseSet = isBaseDiffuseSet;
	clone->isBaseSpecularSet = isBaseSpecularSet;
	clone->SetName(GetName());

	for (auto sf : shaderFeatures)
	{
		clone->Append((ShaderFeature*)sf->Clone());
	}

	return clone;
}
void RenderComponent::Dispose()
{
	DisposeVector(shaderFeatures);
}
