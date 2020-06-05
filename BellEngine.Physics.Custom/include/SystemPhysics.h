#pragma once
#include <ISystemPhysics.h>
#include <IDisposable.h>
#include <IRigidbody.h>
#include <ISoftbody.h>
#include <Triangle3D.h>
#include "Wind.h"
#include "ColliderPlane.h"
#include "ColliderSphere.h"
#include "ColliderCapsule.h"
#include "ColliderBox.h"
#include <atomic>

constexpr unsigned long long DISABLEDEFAULTRESPONSE_SphereToSphere =		0x0000000000000001;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_SphereToPlane =			0x0000000000000010;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_SphereToCapsule =		0x0000000000000100;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_SphereToBox =			0x0000000000001000;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_CapsuleToCapsule =		0x0000000000010000;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_CapsuleToPlane =		0x0000000000100000;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_CapsuleToBox =			0x0000000001000000;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_BoxToBox =				0x0000000010000000;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_BoxToPlane =			0x0000000100000000;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_MeshToMesh =			0x0000001000000000;

class SystemPhysics : public ISystemPhysics, public IDisposable
{
private:
	
	enum class CollisionType_e
	{
		None,

		SphereToSphere,
		SphereToPlane,
		SphereToCapsule,
		SphereToBox,
		CapsuleToCapsule,
		CapsuleToPlane,
		CapsuleToBox,
		BoxToBox,
		BoxToPlane,
		MeshToMesh
	};

	std::atomic<bool> isActive, isUpdateComplete;
	Wind* wind;
	Vector3D gravity;
	float airFriction;
	unsigned long long flags;
	Scene* scene;
	std::vector<Collision_s> collisions;
	IFactory<ICollider>* colliderFactory;
	IFactory<IPhysicsComponent>* physicsComponentFactory;
	std::function<void(Collision_s&)> collisionCallback;

	void OnSphereToSphereCollision(Collision_s& collision);
	void OnSphereToPlaneCollision(Collision_s& collision);
	void OnSphereToCapsuleCollision(Collision_s& collision);
	void OnSphereToBoxCollision(Collision_s& collision);
	void OnCapsuleToCapsuleCollision(Collision_s& collision);
	void OnCapsuleToPlaneCollision(Collision_s& collision);
	void OnCapsuleToBoxCollision(Collision_s& collision);
	void OnBoxToBoxCollision(Collision_s& collision);
	void OnBoxToPlaneCollision(Collision_s& collision);

	const bool TestCollision(ISoftbody*& b, std::vector<Collision_s>& collision);
	const bool TestCollision(IRigidbody*& a, ISoftbody*& b, std::vector<Collision_s>& collision);
	const bool TestCollision(IRigidbody*& a, IRigidbody*& b, std::vector<Collision_s>& collision);
	const bool TestCollision(Entity*& a, std::vector<Collision_s>& collision);
	const bool TestCollision(Entity*& a, Entity*& b, std::vector<Collision_s>& collision);
	void Collide_RBRB(Collision_s& collision);
	void Collide_RBSB(Collision_s& collision);
	void Collide(Collision_s& collision);
	void ClosestPtPointBox(const Vector3D& sphereCenter, ColliderBox* box, Vector3D& closestPoint);
	void ClosestPtPointFixedBox(const Vector3D& sphereCenter, ColliderBox* box, Vector3D& closestPoint);
	const bool TestCapsuleToPlane(ColliderCapsule*& capsule, ColliderPlane*& plane, ParameterContainer& collisionInfo);
	const bool TestCapsuleToCapsule(ColliderCapsule*& capsule0, ColliderCapsule*& capsule1, ParameterContainer& collisionInfo);
	const bool TestSphereToCapsule(ColliderSphere*& sphere, ColliderCapsule*& capsule, ParameterContainer& collisionInfo);
	const bool TestSphereToPlane(ColliderSphere*& sphere, ColliderPlane*& plane, ParameterContainer& collisionInfo);
	const bool TestSphereToSphere(ColliderSphere*& sphere0, ColliderSphere*& sphere1, ParameterContainer& collisionInfo);
	const bool TestSphereToBox(ColliderSphere*& sphere, ColliderBox*& box, ParameterContainer& collisionInfo);
	const bool TestCapsuleToBox(ColliderCapsule*& capsule, ColliderBox*& box, ParameterContainer& collisionInfo);
	const bool TestBoxToPlane(ColliderBox*& box, ColliderPlane*& plane, ParameterContainer& collisionInfo);
	const bool TestBoxToBox(ColliderBox*& box0, ColliderBox*& box1, ParameterContainer& collisionInfo);
	const bool TestMeshToMesh(Entity*& a, Entity*& b);
	const bool TestColliders(Entity*& a, Entity*& b, ParameterContainer& collisionInfo);
	const bool IsCollisionRegistered(const Collision_s& collision);
	void IntegrationStep(IRigidbody*& rigidbody);
	void IntegrationStep(ISoftbody*& softbody);
	void ApplyWind(ISoftbody*& softbody);
	void ApplyWind(IRigidbody*& softbody);
	void ToggleWind(const bool& value);
	const bool IsWindActive();

public:
	SystemPhysics();
	~SystemPhysics();
	
	static bool IsAColliderCollision(ICollider* from, ICollider* to);
	static bool IsPointInCollider(const Vector3D& p, ICollider* collider);
	static void GetClosestTriangleToPosition(const Vector3D& position, Entity& gameObject1, Vector3D& closestPoint, Triangle3D& closestTriangle);
	void SetFlags(const unsigned long long& flags);
	const bool IsUpdateComplete();
	void Activate(const bool& value);
	const bool IsActive();
	void SetWind(const float& radius, const Vector3D& position, const float& maxWindStep, const float& minWindStep);
	const bool GetWind(float& radius, Vector3D& position, float& maxWindStep, float& minWindStep, Vector3D& currentForce);
	void BindCollisionCallback(std::function<void(Collision_s &)> fn);
	void SetScene(Scene*& scene);	
	void SetAirFriction(const float& airFriction);
	const float GetAirFriction();
	void SetGravity(const Vector3D& gravity);
	const Vector3D GetGravity();
	IFactory<ICollider>* GetColliderFactory();
	IFactory<IPhysicsComponent>* GetPhysicsComponentFactory();

	void Update(const FrameTime& deltaTime);
	void Dispose();
};
