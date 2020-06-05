#pragma once
#include <MathLibrary.h>

class IColorable
{
protected:
	Vector4D baseDiffuseColor, baseSpecularColor;
	Vector4D currentDiffuseColor, currentSpecularColor;
	bool isBaseDiffuseSet, isBaseSpecularSet;

	IColorable() :	baseDiffuseColor(), 
					baseSpecularColor(), 
					currentDiffuseColor(), 
					currentSpecularColor(), 
					isBaseDiffuseSet(false), 
					isBaseSpecularSet(false) {};

public:
	virtual ~IColorable() {};

	const Vector4D GetBaseDiffuseColor()
	{
		return this->baseDiffuseColor;
	}
	const Vector4D GetBaseSpecularColor()
	{
		return this->baseSpecularColor;
	}
	void SetDiffuse(const Vector4D& color)
	{
		this->currentDiffuseColor = color;

		if (!isBaseDiffuseSet)
		{
			this->baseDiffuseColor = color;
			isBaseDiffuseSet = true;
		}
	}
	const Vector4D GetDiffuse()
	{
		return currentDiffuseColor;
	}
	void SetSpecular(const Vector4D& color)
	{
		this->currentSpecularColor = color;

		if (!isBaseSpecularSet)
		{
			this->baseSpecularColor = color;
			isBaseSpecularSet = true;
		}
	}
	const Vector4D GetSpecular()
	{
		return currentSpecularColor;
	}	
};