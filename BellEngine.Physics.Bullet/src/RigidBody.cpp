#include "RigidBody.h"
#include "Convert.h"
#include "IBulletColliderWrapper.h"
#include "TransformWrapper.h"
#include <BellEngineID.h>

RigidBody::RigidBody()
{
	parent = nullptr;
	allowCollisionDetection = false;
	isActive = true;
	/// Create Dynamic Objects

	SetTransform(new TransformWrapper());
	((TransformWrapper*)GetTransform())->SetParent(this);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	motionState = new btDefaultMotionState(*((TransformWrapper*)GetTransform())->GetWrappedTransform());
	btScalar mass = 0.0f;

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);
	btVector3 localInertia(0, 0, 0);	
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, nullptr, localInertia);
	rbInfo.m_restitution = 0.5f;

	_rigidbody = new btRigidBody(rbInfo);
	collider = nullptr;
}
RigidBody::~RigidBody()
{
	Dispose();
	//THESE ARE DISPOSED BY THE PHYSICS SYSTEM, BECAUSE... 
	//"BULLET" ~Aliens guy meme

	/*if (_rigidbody)
	{
		delete (_rigidbody);
		_rigidbody = nullptr;
	}*/

	/*if (motionState)
	{
		delete (motionState);
		motionState = nullptr;
	}*/
}

const bool RigidBody::IsFixed()
{
	return GetMass() == 0.0f;
}

void RigidBody::SetCollider(ICollider* collider)
{
	this->collider = collider;

	IBulletColliderWrapper* ptr = dynamic_cast<IBulletColliderWrapper*>(collider);
	if (ptr)
	{
		_rigidbody->setCollisionShape(ptr->GetWrappedShape());
	}
}

ICollider* RigidBody::GetCollider()
{
	return this->collider;
}

void RigidBody::ToggleCollisionDetection(const bool& option)
{
	allowCollisionDetection = option;
}

const bool RigidBody::AllowCollisionDetection()
{
	return allowCollisionDetection;
}

void RigidBody::SetAcceleration(const Vector3D& acceleration)
{
	//this->acceleration = acceleration;
}

const Vector3D RigidBody::GetAcceleration()
{
	return Convert::BtVec3ToVec3(_rigidbody->getTotalForce()) ;// this->acceleration;
}

void RigidBody::SetVelocity(const Vector3D& velocity)
{
	this->_rigidbody->activate(true);
	this->_rigidbody->setLinearVelocity(Convert::Vec3ToBtVec3(velocity));
}

const Vector3D RigidBody::GetVelocity()
{
	btVector3 velocity = this->_rigidbody->getLinearVelocity();
	return Convert::BtVec3ToVec3(velocity);
}

const float RigidBody::GetDampingRatio()
{
	/*if (elasticity != 0.0f)
	{
		return 2 * sqrt(mass * elasticity);
	}
	else
	{
		return 0;
	}*/

	/*bt_rigidbody->getLinearDamping();
	bt_rigidbody->getRestitution();*/

	return _rigidbody->getLinearDamping();
}

void RigidBody::SetElasticity(const float& elasticity)
{
	//this->elasticity = elasticity;
	_rigidbody->setRestitution(elasticity);
}

const float RigidBody::GetInverseMass()
{
	return _rigidbody->getInvMass();
}

void RigidBody::SetMass(const float& mass)
{
	_rigidbody->setMassProps(mass, btVector3());
}

void RigidBody::ApplyForce(const Vector3D& force)
{
	_rigidbody->activate(true);
	_rigidbody->applyCentralForce(Convert::Vec3ToBtVec3(force * 10.0f));
}

btRigidBody* RigidBody::GetWrappedRigidBody()
{
	return this->_rigidbody;
}

const float RigidBody::GetMass()
{
	return _rigidbody->getMass();;
}

const float RigidBody::GetElasticity()
{
	return _rigidbody->getRestitution();
}

ICloneable* RigidBody::Clone()
{
	RigidBody* clone = new RigidBody();	
	clone->allowCollisionDetection = this->allowCollisionDetection;
	clone->collider = (ICollider*)collider->Clone();
	clone->collider->SetParent(clone);
	
	if (clone->_rigidbody)
	{
		delete (clone->_rigidbody);
	}
	clone->_rigidbody = new btRigidBody(*_rigidbody);

	return clone;
}

const std::string RigidBody::GetName()
{
	return this->name;
}

void RigidBody::SetName(const std::string& name)
{
	this->name = name;
}

const std::string RigidBody::GetType()
{
	return RIGIDBODY_STR;
}

const unsigned long long RigidBody::GetTypeID()
{
	return ENGINE_OBJ_ID_PHY_RIGIDBODY;
}

void RigidBody::ClampVelocity()
{
	SetVelocity(Vector3D(0.0f, 0.0f, 0.0f));
}

const bool RigidBody::IsActive()
{
	return isActive;
}

void RigidBody::Activate(const bool& value)
{
	isActive = value;
}

void RigidBody::Dispose()
{

}

void RigidBody::Update(const double& deltaTime)
{
	Transform();
}

void RigidBody::Transform()
{
	/*btTransform t;
	_rigidbody->getMotionState()->getWorldTransform(t);
	SetPosition(Convert::BtVec3ToVec3(t.getOrigin()));
	SetRotation(Convert::BtQuatToQuat(t.getRotation()));*/

	this->collider->Transform(GetWorldMatrix());
}