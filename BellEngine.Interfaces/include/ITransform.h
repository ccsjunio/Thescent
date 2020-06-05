#pragma once
#include <MathLibrary.h>
#include <ICloneable.h>

class ITransform : public ICloneable
{
protected:
	ITransform() {};

public:
	virtual ~ITransform() {};

	virtual void SetOrientation(const Vector3D& orientation) = 0;
	virtual const Vector3D GetOrientation() = 0;
	virtual void SetUp(const Vector3D& up) = 0;
	virtual const Vector3D GetUp() = 0;
	virtual void SetPosition(const Vector3D& position) = 0;
	virtual const Vector3D GetPosition() = 0;
	virtual void SetScale(const Vector3D& scale) = 0;
	virtual const Vector3D GetScale() = 0;
	virtual void SetRotation(const Quaternion& rotation) = 0;
	virtual const Quaternion GetRotation() = 0;
};