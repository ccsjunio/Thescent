/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <MathLibrary.h>
#include "IIdentifiable.h"
#include "ISelectable.h"
#include "ICloneable.h"
#include "ITransformable.h"
#include "ICollider.h"
#include "IComponent.h"

class ILight : public IComponent, public ISelectable, public ITransformable
{
protected:
	ILight()  {};

public:
	virtual ~ILight() {};

	virtual const Vector4D GetBaseDiffuseColor() = 0;
	virtual const Vector4D GetBaseSpecularColor() = 0;
	virtual void SetDiffuse(const Vector4D& color) = 0;
	virtual const Vector4D GetDiffuse() = 0;
	virtual void SetSpecular(const Vector4D& color) = 0;
	virtual const Vector4D GetSpecular() = 0;
	virtual const bool IsVisible() = 0;
	virtual void SetVisibility(const bool& option) = 0;
	virtual void SetCollider(ICollider* collider) = 0;
	virtual ICollider* GetCollider() = 0;
	virtual const std::string GetType() = 0;
	virtual unsigned short GetEnumID() = 0;
	virtual void Update(const double& deltaTime) = 0;
};

