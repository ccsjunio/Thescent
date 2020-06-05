#pragma once
#include <ITransform.h>
#include "IBulletTransformWrapper.h"
#include <IIdentifiable.h>

class TransformWrapper : public ITransform, public IBulletTransformWrapper
{
protected:
	btTransform* transform;
	IIdentifiable* parent;

	Vector3D orientation;
	Vector3D up;
	Vector3D position;
	Vector3D scale;
	Quaternion rotation;

public:
	TransformWrapper();
	~TransformWrapper();

	ICloneable* Clone();
	void SetOrientation(const Vector3D& orientation);
	const Vector3D GetOrientation();
	void SetUp(const Vector3D& up);
	const Vector3D GetUp();
	void SetPosition(const Vector3D& position);
	const Vector3D GetPosition();
	void SetScale(const Vector3D& scale);
	const Vector3D GetScale();
	void SetRotation(const Quaternion& rotation);
	const Quaternion GetRotation();
	btTransform* GetWrappedTransform();

	void SetParent(IIdentifiable* parent)
	{
		this->parent = parent;
	}
	IIdentifiable* GetParent()
	{
		return parent;
	}
};

