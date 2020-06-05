#pragma once
#include <ISystemPhysics.h>
#include "../physics/bullet/btBulletCollisionCommon.h"
#include "../physics/bullet/btBulletDynamicsCommon.h"
#include "BulletWorldInfo.h"
#include <IDisposable.h>
#include <Triangle3D.h>
#include <atomic>

class SystemPhysics : public ISystemPhysics, public IDisposable
{
private:

protected:
	std::atomic<bool> isActive, isUpdateComplete;
	float airFriction;
	Scene* scene;
	IFactory<ICollider>* colliderFactory;
	IFactory<IPhysicsComponent>* physicsComponentFactory;
	std::map<std::string, BulletWorldInfo> worldsPerScene;
	std::function<void(Collision_s&)> collisionCallback;
	unsigned long long flags;

	/*BULLET*/	
	void InitializeBulletWorld();

	bool TestCollision(Entity* a, Entity* b, std::vector<Parameter>& collisionInfo);
	void Collide(Collision_s& collision);

public:
	SystemPhysics();
	~SystemPhysics();
	
	void SetFlags(const unsigned long long& flags);
	const bool IsUpdateComplete();
	void Activate(const bool& value);
	const bool IsActive();

	static bool IsAColliderCollision(ICollider* from, ICollider* to);
	static bool IsPointInCollider(const Vector3D& p, ICollider* collider);
	static void GetClosestTriangleToPosition(const Vector3D& position, Entity& gameObject1, Vector3D& closestPoint, Triangle3D& closestTriangle);
	
	const bool IsWindActive();
	void ToggleWind(const bool& value);
	void SetWind(const float& radius, const Vector3D& position, const float& maxWindStep, const float& minWindStep);
	const bool GetWind(float& radius, Vector3D& position, float& maxWindStep, float& minWindStep, Vector3D& currentForce);
	void BindCollisionCallback(std::function<void(Collision_s&)> fn);
	void SetScene(Scene*& scene);
	void IntegrationStep(Entity* go, const float& deltaTime);
	void SetAirFriction(const float& airFriction);
	const float GetAirFriction();
	void SetGravity(const Vector3D& gravity);
	const Vector3D GetGravity();
	IFactory<ICollider>* GetColliderFactory();
	//IFactory<IRigidbody>* GetRigidbodyFactory();
	IFactory<IPhysicsComponent>* GetPhysicsComponentFactory();
	void Dispose();
	void Update(const FrameTime& deltaTime);
};
