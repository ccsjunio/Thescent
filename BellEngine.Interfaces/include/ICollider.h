#pragma once
/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <ICloneable.h>
#include <IIdentifiable.h>
#include <MathLibrary.h>
#include <ITransform.h>
#include <Parameter.h>
#include <ParameterContainer.h>

class ICollider : public ICloneable, public IIdentifiable
{
protected:
	ICollider(){};

public:
	class IProperties : public ICloneable
	{
	protected:
		IProperties() {};
	public:	
		virtual ~IProperties() {};

		virtual const bool GetExtremes(std::vector<Vector3D>& extremes) = 0;
		virtual ITransform* GetTransform() = 0;
		virtual void SetTransform(ITransform* transform) = 0;
		virtual void GenerateExtremes() = 0;
		virtual void Transform(const Matrix4D& worldMatrix, IProperties* relativeTo) = 0;
	};
	
	virtual ~ICollider() {};

	virtual void SetParent(void* parent) = 0;
	virtual void* GetParent() = 0;
	virtual const bool Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength) = 0;
	virtual const bool Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength, ParameterContainer& output) = 0;
	virtual IProperties* GetProperties() = 0;
	virtual IProperties* GetTransformedProperties() = 0;
	virtual const bool GetContent(const std::string& name, ParameterContainer& data) = 0;
	virtual void Transform(const Matrix4D& worldMatrix) = 0;
};