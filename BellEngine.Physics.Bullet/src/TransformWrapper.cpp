#include "TransformWrapper.h"
#include "Convert.h"
#include "IBulletRigidBodyWrapper.h"

TransformWrapper::TransformWrapper()
{
	parent = nullptr;;
	orientation = Vector3D();
	up = Vector3D();
	position = Vector3D();
	scale = Vector3D();
	rotation = Quaternion();
	transform = new btTransform();
	transform->setIdentity();
	transform->setOrigin(btVector3(0.0f, 0.0f, 0.0f));
}
TransformWrapper::~TransformWrapper()
{
	if (transform)
	{
		delete (transform);
		transform = nullptr;
	}
}

void TransformWrapper::SetOrientation(const Vector3D& orientation)
{
	this->orientation = orientation;
}
void TransformWrapper::SetUp(const Vector3D& up)
{
	this->up = up;
}
void TransformWrapper::SetPosition(const Vector3D& position)
{
	IBulletRigidBodyWrapper* r = dynamic_cast<IBulletRigidBodyWrapper*>(parent);
	if (r)
	{
		transform->setOrigin(Convert::Vec3ToBtVec3(position));
		r->GetWrappedRigidBody()->setWorldTransform(*transform);

		/*transform->setIdentity();
		transform->setOrigin(Convert::Vec3ToBtVec3(position));
		transform->setRotation(transform->getRotation());
		r->GetWrappedRigidBody()->setWorldTransform(*transform);*/

		/*auto t = r->GetWrappedRigidBody()->getWorldTransform();
		t.setOrigin(Convert::Vec3ToBtVec3(position));
		r->GetWrappedRigidBody()->setWorldTransform(t);

		transform = &t;*/
	}

	this->position = position;
}
void TransformWrapper::SetScale(const Vector3D& scale)
{
	this->scale = scale;
}
void TransformWrapper::SetRotation(const Quaternion& rotation)
{
	//IBulletRigidBodyWrapper* r = dynamic_cast<IBulletRigidBodyWrapper*>(parent);
	//if (r)
	//{
	//	transform->setRotation(Convert::QuatToBtQuat(rotation));
	//	r->GetWrappedRigidBody()->setWorldTransform(*transform);

	//	/*transform->setIdentity();
	//	transform->setOrigin(transform->getOrigin());
	//	transform->setRotation(Convert::QuatToBtQuat(rotation));
	//	r->GetWrappedRigidBody()->setWorldTransform(*transform);*/

	//	/*auto t = r->GetWrappedRigidBody()->getWorldTransform();
	//	t.setRotation(Convert::QuatToBtQuat(rotation));
	//	r->GetWrappedRigidBody()->setWorldTransform(t);

	//	transform = &t;*/
	//}

	this->rotation = rotation;
}
const Vector3D TransformWrapper::GetOrientation()
{
	return orientation;
}
const Vector3D TransformWrapper::GetUp()
{
	return up;
}
const Vector3D TransformWrapper::GetPosition()
{
	IBulletRigidBodyWrapper* r = dynamic_cast<IBulletRigidBodyWrapper*>(parent);
	if (r)
	{
		auto t = r->GetWrappedRigidBody()->getWorldTransform();
		return Convert::BtVec3ToVec3(t.getOrigin());
	}
	else
	{
		return position;
	}

	//return position;
}
const Vector3D TransformWrapper::GetScale()
{
	return scale;
}
const Quaternion TransformWrapper::GetRotation()
{
	/*IBulletRigidBodyWrapper* r = dynamic_cast<IBulletRigidBodyWrapper*>(parent);
	if (r)
	{
		auto t = r->GetWrappedRigidBody()->getWorldTransform();
		return Convert::BtQuatToQuat(t.getRotation());
	}
	else
	{
		return rotation;
	}*/

	return rotation;
}
ICloneable* TransformWrapper::Clone()
{
	TransformWrapper* t = new TransformWrapper();
	t->transform = new btTransform(*transform);
	t->orientation = orientation;
	t->up = up;
	t->position = this->position;
	t->rotation = this->rotation;
	t->scale = this->scale;

	return t;
};

btTransform* TransformWrapper::GetWrappedTransform()
{
	return transform;
}