#pragma once
#include <IComponent.h>
#include <MathLibrary.h>
#include <ICollider.h>
#include <ITransformable.h>
#include <ICloneable.h>
#include "IPhysicsComponent.h"

class IRigidbody : public IPhysicsComponent, public ITransformable
{
protected:
	IRigidbody() {};

public:
	virtual ~IRigidbody() {};

	void MoveRL(const float& factor, const double& deltaTime)
	{
		Vector3D sideVector = glm::cross(GetForwardInWorld(), GetUpVector());

		Vector3D vecAmountToMove = glm::normalize(sideVector) * factor;
		SetVelocity(vecAmountToMove);
		SetPosition(GetPosition() + GetVelocity() * (float)deltaTime);
	}
	void MoveUD(const float& factor, const double& deltaTime)
	{
		Vector3D vecAmountToMove = glm::normalize(GetUpVector()) * factor;
		SetVelocity(vecAmountToMove);
		SetPosition(GetPosition() + GetVelocity() * (float)deltaTime);
	}
	void MoveFB(const float& factor, const double& deltaTime)
	{
		SetVelocity(GetForwardInWorld() * factor);
		SetPosition(GetPosition() + GetVelocity() * (float)deltaTime);
	}

	virtual const bool IsFixed() = 0;
	virtual void SetCollider(ICollider* collider) = 0;
	virtual ICollider* GetCollider() = 0;
	virtual void SetAcceleration(const Vector3D& acceleration) = 0;
	virtual const Vector3D GetAcceleration() = 0;
	virtual void SetVelocity(const Vector3D& velocity) = 0;
	virtual const Vector3D GetVelocity() = 0;
	virtual const float GetDampingRatio() = 0;
	virtual void SetElasticity(const float& elasticity) = 0;
	virtual const float GetElasticity() = 0;
	virtual const float GetInverseMass() = 0;
	virtual void SetMass(const float& mass) = 0;
	virtual const float GetMass() = 0;
	virtual void ApplyForce(const Vector3D& force) = 0;
	virtual void ClampVelocity() = 0;
	virtual void Transform() = 0;
	virtual void Update(const double& deltaTime) = 0;
};