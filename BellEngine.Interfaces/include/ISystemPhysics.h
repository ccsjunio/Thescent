#pragma once
#include "ISystem.h"
#include "ICollider.h"
#include "IPhysicsComponent.h"
#include "Parameter.h"
#include "Scene.h"
#include "IFactory.h"
#include "ParameterContainer.h"
#include <vector>
#include <functional>

#define INFINITE_MASS 0.0f
#define INFINITE_INVERSEMASS 1.0f

class ISystemPhysics : public ISystem
{
protected:
	ISystemPhysics() {};

public:
	/*Used for propagating collision in callbacks.*/
	struct Collision_s
	{
	public:
		static unsigned long long globalCollisionID;

		unsigned long long collisionID, collisionType;
		Entity* a, * b;
		ParameterContainer collisionInfo;

		Collision_s()
		{
			this->a = nullptr;
			this->b = nullptr;
			this->collisionID = globalCollisionID;
			this->collisionType = 0;

			if (globalCollisionID == LLONG_MAX)
			{
				globalCollisionID = 0;
			}
			else
			{
				globalCollisionID++;
			}
		};
		Collision_s(Entity*& a, Entity*& b, ParameterContainer& collisionInfo, unsigned long long collisionType)
		{
			this->a = a;
			this->b = b;
			this->collisionType = collisionType;
			this->collisionInfo = collisionInfo;
			this->collisionID = globalCollisionID;

			if (globalCollisionID == LLONG_MAX)
			{
				globalCollisionID = 0;
			}
			else
			{
				globalCollisionID++;
			}
		}
		Collision_s(IPhysicsComponent*& a, IPhysicsComponent*& b, ParameterContainer& collisionInfo, unsigned long long collisionType)
		{
			this->a = a->GetParent<Entity*>();
			this->b = b->GetParent<Entity*>();
			this->collisionType = collisionType;
			this->collisionInfo = collisionInfo;
			this->collisionID = globalCollisionID;

			if (globalCollisionID == LLONG_MAX)
			{
				globalCollisionID = 0;
			}
			else
			{
				globalCollisionID++;
			}
		}
		const bool FindCollisionInfoByTag(const std::string& name, Parameter& info)
		{
			return collisionInfo.FindParameterByName(name, info);
		}
		const bool IsDisposing()
		{
			return a->IsDisposing() || b->IsDisposing();
		}
	};

	virtual ~ISystemPhysics() {};
	virtual void SetFlags(const unsigned long long& flags) = 0;
	virtual void BindCollisionCallback(std::function<void(Collision_s&)> fn) = 0;
	virtual void SetScene(Scene*& scene) = 0;
	virtual void SetAirFriction(const float& airFriction) = 0;
	virtual void ToggleWind(const bool& value) = 0;
	virtual const bool IsWindActive() = 0;
	virtual void SetWind(const float& radius, const Vector3D& position, const float& maxWindStep, const float& minWindStep) = 0;
	virtual const bool GetWind(float& radius, Vector3D& position, float& maxWindStep, float& minWindStep, Vector3D& currentForce) = 0;
	virtual const float GetAirFriction() = 0;
	virtual void SetGravity(const Vector3D& gravity) = 0;
	virtual const Vector3D GetGravity() = 0;
	virtual IFactory<ICollider>* GetColliderFactory() = 0;
	virtual IFactory<IPhysicsComponent>* GetPhysicsComponentFactory() = 0;
};