/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <string>
#include <IRigidbody.h>
#include <atomic>

#define RIGIDBODY_STR "RigidBody"

class RigidBody : public IRigidbody
{
protected:
	std::string name;
	float inverseMass;
	float mass;
	float elasticity;
	Vector3D velocity;
	Vector3D acceleration;
	std::atomic<bool> allowCollisionDetection;
	ICollider* collider;
	std::atomic<bool> isActive;

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
	void ToggleCollisionDetection(const bool& value);
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
	void ClampVelocity();
	void Transform();
	void Dispose();
	void Update(const double& deltaTime);
};