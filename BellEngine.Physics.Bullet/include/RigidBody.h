#pragma once
#include <string>
#include <IRigidbody.h>
#include "IBulletRigidBodyWrapper.h"

#define RIGIDBODY_STR "RigidBody"

class RigidBody : public IRigidbody, public IBulletRigidBodyWrapper
{
protected:
	btDefaultMotionState* motionState;
	btRigidBody* _rigidbody;
	bool isActive;
	bool allowCollisionDetection;
	ICollider* collider;
	void* parent;
	std::string name;

public:
	RigidBody();
	~RigidBody();
	
	const bool IsFixed();
	const std::string GetName();
	void SetName(const std::string& name);
	const std::string GetType();
	const unsigned long long GetTypeID();
	ICloneable* Clone();
	const bool IsActive();
	void Activate(const bool& value = true);
	void SetCollider(ICollider* collider);
	ICollider* GetCollider();
	void ToggleCollisionDetection(const bool& option);
	const bool AllowCollisionDetection();
	void SetAcceleration(const Vector3D& acceleration);
	const Vector3D GetAcceleration();
	void SetVelocity(const Vector3D& velocity);
	const Vector3D GetVelocity();
	const float GetDampingRatio();
	void SetElasticity(const float& elasticity);
	const float GetElasticity();
	const float GetInverseMass();
	void SetMass(const float& mass);
	const float GetMass();
	void ApplyForce(const Vector3D& force);
	btRigidBody* GetWrappedRigidBody();
	void Transform();
	void ClampVelocity();
	void Dispose();
	void Update(const double& deltaTime);
};