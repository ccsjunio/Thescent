#pragma once
#include <ShaderFeature.h>

class IRenderable
{
private:
	bool isVisible;
	int renderMode;
	
protected:
	IRenderable() : isVisible(true), renderMode(0) {};
	void DisposeShaderFeatures()
	{
		for (ShaderFeature* sf : shaderFeatures)
		{
			if (sf)
			{
				delete sf;
				sf = nullptr;
			}
		}
		shaderFeatures.clear();
		shaderFeatures.shrink_to_fit();
	}

public:
	std::vector<ShaderFeature*> shaderFeatures;

	virtual ~IRenderable() {};

	const bool IsVisible() 
	{
		return isVisible; 
	}
	void SetVisibility(const bool& option)
	{
		isVisible = option;
	}
	void SetRenderMode(const int& renderMode)
	{
		this->renderMode = renderMode;
	}
	const int GetRenderMode()
	{
		return renderMode;
	}
	void GetShaderFeatures(std::vector<ShaderFeature*>& shaderFeatures)
	{
		shaderFeatures = this->shaderFeatures;
	}
	void Append(ShaderFeature* sf)
	{
		shaderFeatures.push_back(sf);
	}
	void Remove(ShaderFeature* sf)
	{

	}
};