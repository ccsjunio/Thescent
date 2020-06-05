/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <MathLibrary.h>
#include "IIdentifiable.h"
#include "Entity.h"
#include "IUpdatable.h"

class ICamera : public IIdentifiable, public IUpdatable
{
protected:
	ICamera() {};

public:
	virtual ~ICamera() {};
	
	virtual const Matrix4D GetViewMatrix() = 0;
	virtual const Matrix4D GetProjectionMatrix(const float& aspectRatio) = 0;

	virtual const Vector3D GetTargetPosition() = 0;
	virtual std::string GetTargetName() = 0;
	virtual const Vector3D GetForward() = 0;

	virtual void SetEyePosition(const Vector3D& eyePosition) = 0;
	virtual const Vector3D GetEyePosition() = 0;
	virtual void SetFarClippingPlane(const float& farClippingPlane) = 0;
	virtual const float GetFarClippingPlane() = 0;
	virtual void SetNearClippingPlane(const float& nearClippingPlane) = 0;
	virtual const float GetNearClippingPlane() = 0;
	virtual void SetFOV(const float& fov) = 0;
	virtual const float GetFOV() = 0;
	virtual void SetUp(const Vector3D& up) = 0;
	virtual const Vector3D GetUp() = 0;
	virtual const bool IsFacingEntity(Entity* entity) = 0;
};

