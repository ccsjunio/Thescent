#include "SystemPhysics.h"
#include "PhysicsComponentFactory.h"
#include "Softbody.h"
#include "ColliderFactory.h"
#include "Integrator.h"
#include "ColliderAABB.h"
#include "ColliderSphere.h"
#include "ColliderBox.h"
#include "ColliderCapsule.h"
#include "ColliderPlane.h"
#include <MathHelper.h>
#include <RigidBody.h>
#include <IRender.h>
#include <IMeshComponent.h>
#include <BellEngineID.h>
#include <StringHelpers.h>

//#define ENABLE_SOFTBODY_INTERNAL_COLLISION
#define ENABLE_FINITE_PLANES
#define COLLISION_TYPE_RB_RB 0
#define COLLISION_TYPE_RB_SB 1

unsigned long long ISystemPhysics::Collision_s::globalCollisionID = 0;

/*Standard sphere-sphere collision response.*/
void SystemPhysics::OnSphereToSphereCollision(Collision_s& collision)
{
	Parameter p;
	if (!collision.collisionInfo.FindParameterByName("RigidbodyA", p)) return;

	IRigidbody* aRB = (IRigidbody*)p.valuePtr;
	if (!aRB) return;
	if (aRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERSPHERE) return;

	if (!collision.collisionInfo.FindParameterByName("RigidbodyB", p)) return;

	IRigidbody* bRB = (IRigidbody*)p.valuePtr;
	if (!bRB) return;
	if (bRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERSPHERE) return;

	ColliderSphere* sphereA = (ColliderSphere*)aRB->GetCollider();
	if (!sphereA) return;

	ColliderSphere* sphereB = (ColliderSphere*)bRB->GetCollider();
	if (!sphereB) return;

	//https://en.wikipedia.org/wiki/Inelastic_collision
	Vector3D AvelocityVector = glm::normalize(aRB->GetVelocity());
	Vector3D BvelocityVector = glm::normalize(bRB->GetVelocity());
	Vector3D AvelocityFinal = Vector3D();
	Vector3D BvelocityFinal = Vector3D();
	float c = 0.1f;
	float ma = aRB->GetMass();
	float mb = bRB->GetMass();
	Vector3D v0_A = aRB->GetVelocity();
	Vector3D v0_B = bRB->GetVelocity();
	AvelocityFinal = (c * mb * (v0_B - v0_A) + ma * v0_A + mb * v0_B) / (ma + mb);
	BvelocityFinal = (c * mb * (v0_A - v0_B) + ma * v0_A + mb * v0_B) / (ma + mb);

	bool findOverlap = collision.FindCollisionInfoByTag("overlapdistance", p);
	float overlapDistance = 1.0f;
	if (findOverlap)
	{
		overlapDistance = strtof(p.value.c_str(), NULL);
	}

	if (!aRB->IsFixed())
	{
		Vector3D p = sphereA->GetTransformedProperties()->GetTransform()->GetPosition();
		sphereA->GetTransformedProperties()->GetTransform()->SetPosition(p + overlapDistance * glm::normalize(bRB->GetPosition() - aRB->GetPosition()));
		aRB->SetPosition(aRB->GetPosition() + overlapDistance * glm::normalize(bRB->GetPosition() - aRB->GetPosition()));

		Vector3D reflectionVec = glm::reflect(AvelocityVector, BvelocityVector);
		reflectionVec = glm::normalize(reflectionVec);

		//Gets length of the speed vector
		float speed = glm::length(AvelocityFinal);

		//Bounce off the ball by changing the direction
		aRB->SetVelocity(reflectionVec * speed);

		//Damp speed of ball
		float damping = aRB->GetDampingRatio();
		//aRB->velocity = aRB->velocity / damping;
		aRB->SetVelocity(AvelocityFinal * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
	}

	if (!bRB->IsFixed())
	{
		Vector3D p = sphereB->GetTransformedProperties()->GetTransform()->GetPosition();
		sphereB->GetTransformedProperties()->GetTransform()->SetPosition(p + overlapDistance * glm::normalize(aRB->GetPosition() - bRB->GetPosition()));
		bRB->SetPosition(bRB->GetPosition() + overlapDistance * glm::normalize(aRB->GetPosition() - bRB->GetPosition()));

		Vector3D reflectionVec = glm::reflect(BvelocityVector, AvelocityVector);
		reflectionVec = glm::normalize(reflectionVec);

		//Gets length of the speed vector
		float speed = glm::length(BvelocityFinal);

		//Bounce off the ball by changing the direction
		bRB->SetVelocity(reflectionVec * speed);

		//Damp speed of ball
		float damping = bRB->GetDampingRatio();
		//bRB->velocity = bRB->velocity / damping;
		bRB->SetVelocity(BvelocityFinal * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
	}
}
/*Standard sphere-plane collision response.*/
void SystemPhysics::OnSphereToPlaneCollision(Collision_s& collision)
{
	Parameter p;
	if (!collision.collisionInfo.FindParameterByName("RigidbodyA", p)) return;

	IRigidbody* aRB = (IRigidbody*)p.valuePtr;
	if (!aRB) return;
	if (aRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERSPHERE) return;

	if (!collision.collisionInfo.FindParameterByName("RigidbodyB", p)) return;

	IRigidbody* bRB = (IRigidbody*)p.valuePtr;
	if (!bRB) return;
	if (bRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERPLANE) return;

	ColliderSphere* sphereCollider = (ColliderSphere*)aRB->GetCollider();
	if (!sphereCollider) return;

	ColliderPlane* planeCollider = (ColliderPlane*)bRB->GetCollider();
	if (!planeCollider) return;

	if (!aRB->IsFixed())
	{
		//Bounce
		Vector3D velocityVector = glm::normalize(aRB->GetVelocity());
		Vector3D planeNormal = ((ColliderPlane::Properties*)((ColliderPlane*)bRB->GetCollider())->GetTransformedProperties())->normal;

		bool findOverlap = collision.FindCollisionInfoByTag("overlapdistance", p);
		if (findOverlap)
		{
			float overlapDist = strtof(p.value.c_str(), NULL);
			Vector3D p = sphereCollider->GetTransformedProperties()->GetTransform()->GetPosition();
			sphereCollider->GetTransformedProperties()->GetTransform()->SetPosition(p + (overlapDist * planeNormal));
			aRB->SetPosition(aRB->GetPosition() + (overlapDist * planeNormal));
		}

		Vector3D reflectionVec = glm::reflect(velocityVector, planeNormal);
		reflectionVec = glm::normalize(reflectionVec);

		//Gets length of the speed vector
		float speed = glm::length(aRB->GetVelocity());

		//Bounce off the ball by changing the direction
		aRB->SetVelocity(reflectionVec * speed /** (float)deltaTime*/);

		//Damp speed of ball
		float damping = aRB->GetDampingRatio();
		//aRB->velocity = aRB->velocity / damping;
		aRB->SetVelocity(aRB->GetVelocity() * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
	}
}
/*Standard sphere-capsule collision response.*/
void SystemPhysics::OnSphereToCapsuleCollision(Collision_s& collision)
{
	Parameter p;
	if (!collision.collisionInfo.FindParameterByName("RigidbodyA", p)) return;

	IRigidbody* aRB = (IRigidbody*)p.valuePtr;
	if (!aRB) return;
	if (aRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERSPHERE) return;

	if (!collision.collisionInfo.FindParameterByName("RigidbodyB", p)) return;

	IRigidbody* bRB = (IRigidbody*)p.valuePtr;
	if (!bRB) return;
	if (bRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERCAPSULE) return;

	ColliderSphere* sphere = (ColliderSphere*)aRB->GetCollider();
	if (!sphere) return;

	ColliderCapsule* capsule = (ColliderCapsule*)bRB->GetCollider();
	if (!capsule) return;

	//https://en.wikipedia.org/wiki/Inelastic_collision
	Vector3D AvelocityVector = glm::normalize(aRB->GetVelocity());
	Vector3D BvelocityVector = glm::normalize(bRB->GetVelocity());
	Vector3D AvelocityFinal = Vector3D();
	Vector3D BvelocityFinal = Vector3D();
	float c = 0.1f;
	float ma = aRB->GetMass();
	float mb = bRB->GetMass();
	Vector3D v0_A = aRB->GetVelocity();
	Vector3D v0_B = bRB->GetVelocity();
	AvelocityFinal = (c * mb * (v0_B - v0_A) + ma * v0_A + mb * v0_B) / (ma + mb);
	BvelocityFinal = (c * mb * (v0_A - v0_B) + ma * v0_A + mb * v0_B) / (ma + mb);

	bool findOverlap = collision.FindCollisionInfoByTag("overlapdistance", p);
	float overlapDistance = 1.0f;
	if (findOverlap)
	{
		overlapDistance = strtof(p.value.c_str(), NULL);
	}

	if (!aRB->IsFixed())
	{
		Vector3D p = sphere->GetTransformedProperties()->GetTransform()->GetPosition();
		sphere->GetTransformedProperties()->GetTransform()->SetPosition(p + overlapDistance * glm::normalize(bRB->GetPosition() - aRB->GetPosition()));
		aRB->SetPosition(aRB->GetPosition() + overlapDistance * glm::normalize(bRB->GetPosition() - aRB->GetPosition()));

		Vector3D reflectionVec = glm::reflect(AvelocityVector, BvelocityVector);
		reflectionVec = glm::normalize(reflectionVec);

		//Gets length of the speed vector
		float speed = glm::length(AvelocityFinal);

		//Bounce off the ball by changing the direction
		aRB->SetVelocity(reflectionVec * speed);

		//Damp speed of ball
		float damping = aRB->GetDampingRatio();
		//aRB->velocity = aRB->velocity / damping;
		aRB->SetVelocity(AvelocityFinal * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
	}

	if (!bRB->IsFixed())
	{
		Vector3D p = capsule->GetTransformedProperties()->GetTransform()->GetPosition();
		capsule->GetTransformedProperties()->GetTransform()->SetPosition(p + overlapDistance * glm::normalize(aRB->GetPosition() - bRB->GetPosition()));
		bRB->SetPosition(bRB->GetPosition() + overlapDistance * glm::normalize(aRB->GetPosition() - bRB->GetPosition()));

		Vector3D reflectionVec = glm::reflect(BvelocityVector, AvelocityVector);
		reflectionVec = glm::normalize(reflectionVec);

		//Gets length of the speed vector
		float speed = glm::length(BvelocityFinal);

		//Bounce off the ball by changing the direction
		bRB->SetVelocity(reflectionVec * speed);

		//Damp speed of ball
		float damping = bRB->GetDampingRatio();
		//bRB->velocity = bRB->velocity / damping;
		bRB->SetVelocity(BvelocityFinal * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
	}
}
/*Standard sphere-box collision response.*/
void SystemPhysics::OnSphereToBoxCollision(Collision_s& collision)
{
	Parameter p;
	if (!collision.collisionInfo.FindParameterByName("RigidbodyA", p)) return;

	IRigidbody* aRB = (IRigidbody*)p.valuePtr;
	if (!aRB) return;
	if (aRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERSPHERE) return;

	if (!collision.collisionInfo.FindParameterByName("RigidbodyB", p)) return;

	IRigidbody* bRB = (IRigidbody*)p.valuePtr;
	if (!bRB) return;
	if (bRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERBOX) return;

	ColliderSphere* sphere = (ColliderSphere*)aRB->GetCollider();
	if (!sphere) return;

	ColliderBox* box = (ColliderBox*)bRB->GetCollider();
	if (!box) return;

	//https://en.wikipedia.org/wiki/Inelastic_collision
	Vector3D AvelocityVector = glm::normalize(aRB->GetVelocity());
	Vector3D BvelocityVector = glm::normalize(bRB->GetVelocity());
	Vector3D AvelocityFinal = Vector3D();
	Vector3D BvelocityFinal = Vector3D();
	float c = 0.1f;
	float ma = aRB->GetMass();
	float mb = bRB->GetMass();
	Vector3D v0_A = aRB->GetVelocity();
	Vector3D v0_B = bRB->GetVelocity();
	AvelocityFinal = (c * mb * (v0_B - v0_A) + ma * v0_A + mb * v0_B) / (ma + mb);
	BvelocityFinal = (c * mb * (v0_A - v0_B) + ma * v0_A + mb * v0_B) / (ma + mb);

	bool findOverlap = collision.FindCollisionInfoByTag("overlapdistance", p);
	float overlapDistance = 1.0f;
	if (findOverlap)
	{
		overlapDistance = strtof(p.value.c_str(), NULL);
	}

	Vector3D intersectionPoint = bRB->GetPosition();
	if (collision.FindCollisionInfoByTag("intersectionPoint", p))
	{
		intersectionPoint = StringHelpers::StrToVec3(p.value);
	}

	if (!aRB->IsFixed())
	{	
		//sphere_pos += result.normal * (radius - distance_sphere_contact_point)
		Vector3D p = aRB->GetPosition();
		Vector3D collisionNormal = glm::normalize(p - intersectionPoint);
		p = p + overlapDistance * collisionNormal;

		sphere->GetTransformedProperties()->GetTransform()->SetPosition(p);
		aRB->SetPosition(p);

		//aRB->ClampVelocity();

		Vector3D reflectionVec = glm::reflect(AvelocityVector, BvelocityVector);
		reflectionVec = glm::normalize(reflectionVec);

		//Gets length of the speed vector
		float speed = glm::length(AvelocityFinal);

		//Bounce off the ball by changing the direction
		aRB->SetVelocity(reflectionVec * speed);

		//Damp speed of ball
		float damping = aRB->GetDampingRatio();
		//aRB->velocity = aRB->velocity / damping;
		aRB->SetVelocity(AvelocityFinal * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
	}

	if (!bRB->IsFixed())
	{
		Vector3D p = box->GetTransformedProperties()->GetTransform()->GetPosition();
		box->GetTransformedProperties()->GetTransform()->SetPosition(p + overlapDistance * glm::normalize(p - intersectionPoint));
		bRB->SetPosition(bRB->GetPosition() + overlapDistance * glm::normalize(aRB->GetPosition() - intersectionPoint));

		Vector3D reflectionVec = glm::reflect(BvelocityVector, AvelocityVector);
		reflectionVec = glm::normalize(reflectionVec);

		//Gets length of the speed vector
		float speed = glm::length(BvelocityFinal);

		//Bounce off the ball by changing the direction
		bRB->SetVelocity(reflectionVec * speed);

		//Damp speed of ball
		float damping = bRB->GetDampingRatio();
		//bRB->velocity = bRB->velocity / damping;
		bRB->SetVelocity(BvelocityFinal * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
	}
}
/*Standard capsule-capsule collision response.*/
void SystemPhysics::OnCapsuleToCapsuleCollision(Collision_s& collision)
{
	Parameter p;
	if (!collision.collisionInfo.FindParameterByName("RigidbodyA", p)) return;

	IRigidbody* aRB = (IRigidbody*)p.valuePtr;
	if (!aRB) return;
	if (aRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERCAPSULE) return;

	if (!collision.collisionInfo.FindParameterByName("RigidbodyB", p)) return;

	IRigidbody* bRB = (IRigidbody*)p.valuePtr;
	if (!bRB) return;
	if (bRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERCAPSULE) return;

	ColliderCapsule* capsuleA = (ColliderCapsule*)aRB->GetCollider();
	if (!capsuleA) return;

	ColliderCapsule* capsuleb = (ColliderCapsule*)bRB->GetCollider();
	if (!capsuleb) return;

	//https://en.wikipedia.org/wiki/Inelastic_collision
	Vector3D AvelocityVector = glm::normalize(aRB->GetVelocity());
	Vector3D BvelocityVector = glm::normalize(bRB->GetVelocity());
	Vector3D AvelocityFinal = Vector3D();
	Vector3D BvelocityFinal = Vector3D();
	float c = 0.1f;
	float ma = aRB->GetMass();
	float mb = bRB->GetMass();
	Vector3D v0_A = aRB->GetVelocity();
	Vector3D v0_B = bRB->GetVelocity();
	AvelocityFinal = (c * mb * (v0_B - v0_A) + ma * v0_A + mb * v0_B) / (ma + mb);
	BvelocityFinal = (c * mb * (v0_A - v0_B) + ma * v0_A + mb * v0_B) / (ma + mb);

	bool findOverlap = collision.FindCollisionInfoByTag("overlapdistance", p);
	float overlapDistance = 1.0f;
	if (findOverlap)
	{
		overlapDistance = strtof(p.value.c_str(), NULL);
	}

	if (!aRB->IsFixed())
	{
		Vector3D p = capsuleA->GetTransformedProperties()->GetTransform()->GetPosition();
		capsuleA->GetTransformedProperties()->GetTransform()->SetPosition(p + overlapDistance * glm::normalize(bRB->GetPosition() - aRB->GetPosition()));
		aRB->SetPosition(aRB->GetPosition() + overlapDistance * glm::normalize(bRB->GetPosition() - aRB->GetPosition()));

		Vector3D reflectionVec = glm::reflect(AvelocityVector, BvelocityVector);
		reflectionVec = glm::normalize(reflectionVec);

		//Gets length of the speed vector
		float speed = glm::length(AvelocityFinal);

		//Bounce off the ball by changing the direction
		aRB->SetVelocity(reflectionVec * speed);

		//Damp speed of ball
		float damping = aRB->GetDampingRatio();
		//aRB->velocity = aRB->velocity / damping;
		aRB->SetVelocity(AvelocityFinal * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
	}

	if (!bRB->IsFixed())
	{
		Vector3D p = capsuleb->GetTransformedProperties()->GetTransform()->GetPosition();
		capsuleb->GetTransformedProperties()->GetTransform()->SetPosition(p + overlapDistance * glm::normalize(aRB->GetPosition() - bRB->GetPosition()));
		bRB->SetPosition(bRB->GetPosition() + overlapDistance * glm::normalize(aRB->GetPosition() - bRB->GetPosition()));

		Vector3D reflectionVec = glm::reflect(BvelocityVector, AvelocityVector);
		reflectionVec = glm::normalize(reflectionVec);

		//Gets length of the speed vector
		float speed = glm::length(BvelocityFinal);

		//Bounce off the ball by changing the direction
		bRB->SetVelocity(reflectionVec * speed);

		//Damp speed of ball
		float damping = bRB->GetDampingRatio();
		//bRB->velocity = bRB->velocity / damping;
		bRB->SetVelocity(BvelocityFinal * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
	}
}
/*Standard capsule-plane collision response.*/
void SystemPhysics::OnCapsuleToPlaneCollision(Collision_s& collision)
{
	Parameter p;
	if (!collision.collisionInfo.FindParameterByName("RigidbodyA", p)) return;

	IRigidbody* aRB = (IRigidbody*)p.valuePtr;
	if (!aRB) return;
	if (aRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERCAPSULE) return;

	if (!collision.collisionInfo.FindParameterByName("RigidbodyB", p)) return;

	IRigidbody* bRB = (IRigidbody*)p.valuePtr;
	if (!bRB) return;
	if (bRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERPLANE) return;

	ColliderCapsule* capsuleCollider = (ColliderCapsule*)aRB->GetCollider();
	if (!capsuleCollider) return;

	ColliderPlane* planeCollider = (ColliderPlane*)bRB->GetCollider();
	if (!planeCollider) return;

	if (!aRB->IsFixed())
	{
		//Bounce
		Vector3D velocityVector = glm::normalize(aRB->GetVelocity());
		Vector3D planeNormal = ((ColliderPlane::Properties*)((ColliderPlane*)bRB->GetCollider())->GetTransformedProperties())->normal;

		bool findOverlap = collision.FindCollisionInfoByTag("overlapdistance", p);
		if (findOverlap)
		{
			float overlapDist = strtof(p.value.c_str(), NULL);
			Vector3D p = capsuleCollider->GetTransformedProperties()->GetTransform()->GetPosition();
			capsuleCollider->GetTransformedProperties()->GetTransform()->SetPosition(p + (overlapDist * planeNormal));
			aRB->SetPosition(aRB->GetPosition() + (overlapDist * planeNormal));
		}

		Vector3D reflectionVec = glm::reflect(velocityVector, planeNormal);
		reflectionVec = glm::normalize(reflectionVec);

		//Gets length of the speed vector
		float speed = glm::length(aRB->GetVelocity());

		//Bounce off the ball by changing the direction
		aRB->SetVelocity(reflectionVec * speed /** (float)deltaTime*/);

		//Damp speed of ball
		float damping = aRB->GetDampingRatio();
		//aRB->velocity = aRB->velocity / damping;
		aRB->SetVelocity(aRB->GetVelocity() * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
	}
}
/*Standard capsule-box collision response.*/
void SystemPhysics::OnCapsuleToBoxCollision(Collision_s& collision)
{
}
/*Standard box-box collision response.*/
void SystemPhysics::OnBoxToBoxCollision(Collision_s& collision)
{
	Parameter p;
	if (!collision.collisionInfo.FindParameterByName("RigidbodyA", p)) return;

	IRigidbody* aRB = (IRigidbody*)p.valuePtr;
	if (!aRB) return;
	if (aRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERBOX) return;

	if (!collision.collisionInfo.FindParameterByName("RigidbodyB", p)) return;

	IRigidbody* bRB = (IRigidbody*)p.valuePtr;
	if (!bRB) return;
	if (bRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERBOX) return;

	ColliderBox* boxA = (ColliderBox*)aRB->GetCollider();
	if (!boxA) return;

	ColliderBox* boxB = (ColliderBox*)bRB->GetCollider();
	if (!boxB) return;

	//https://en.wikipedia.org/wiki/Inelastic_collision
	Vector3D AvelocityVector = glm::normalize(aRB->GetVelocity());
	Vector3D BvelocityVector = glm::normalize(bRB->GetVelocity());
	Vector3D AvelocityFinal = Vector3D();
	Vector3D BvelocityFinal = Vector3D();
	float c = 0.1f;
	float ma = aRB->GetMass();
	float mb = bRB->GetMass();
	Vector3D v0_A = aRB->GetVelocity();
	Vector3D v0_B = bRB->GetVelocity();
	AvelocityFinal = (c * mb * (v0_B - v0_A) + ma * v0_A + mb * v0_B) / (ma + mb);
	BvelocityFinal = (c * mb * (v0_A - v0_B) + ma * v0_A + mb * v0_B) / (ma + mb);

	bool findOverlap = collision.FindCollisionInfoByTag("overlapdistance", p);
	float overlapDistance = 1.0f;
	if (findOverlap)
	{
		overlapDistance = strtof(p.value.c_str(), NULL);
	}

	if (!aRB->IsFixed())
	{
		Vector3D p = boxA->GetTransformedProperties()->GetTransform()->GetPosition();
		boxA->GetTransformedProperties()->GetTransform()->SetPosition(p + overlapDistance * glm::normalize(bRB->GetPosition() - aRB->GetPosition()));
		aRB->SetPosition(aRB->GetPosition() + overlapDistance * glm::normalize(bRB->GetPosition() - aRB->GetPosition()));

		Vector3D reflectionVec = glm::reflect(AvelocityVector, BvelocityVector);
		reflectionVec = glm::normalize(reflectionVec);

		//Gets length of the speed vector
		float speed = glm::length(AvelocityFinal);

		//Bounce off the ball by changing the direction
		aRB->SetVelocity(reflectionVec * speed);

		//Damp speed of ball
		float damping = aRB->GetDampingRatio();
		//aRB->velocity = aRB->velocity / damping;
		aRB->SetVelocity(AvelocityFinal * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
	}

	if (!bRB->IsFixed())
	{
		Vector3D p = boxB->GetTransformedProperties()->GetTransform()->GetPosition();
		boxB->GetTransformedProperties()->GetTransform()->SetPosition(p + overlapDistance * glm::normalize(aRB->GetPosition() - bRB->GetPosition()));
		bRB->SetPosition(bRB->GetPosition() + overlapDistance * glm::normalize(aRB->GetPosition() - bRB->GetPosition()));

		Vector3D reflectionVec = glm::reflect(BvelocityVector, AvelocityVector);
		reflectionVec = glm::normalize(reflectionVec);

		//Gets length of the speed vector
		float speed = glm::length(BvelocityFinal);

		//Bounce off the ball by changing the direction
		bRB->SetVelocity(reflectionVec * speed);

		//Damp speed of ball
		float damping = bRB->GetDampingRatio();
		//bRB->velocity = bRB->velocity / damping;
		bRB->SetVelocity(BvelocityFinal * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
	}
}
/*Standard box-plane collision response.*/
void SystemPhysics::OnBoxToPlaneCollision(Collision_s& collision)
{
	Parameter p;
	if (!collision.collisionInfo.FindParameterByName("RigidbodyA", p)) return;

	IRigidbody* aRB = (IRigidbody*)p.valuePtr;
	if (!aRB) return;
	if (aRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERBOX) return;

	if (!collision.collisionInfo.FindParameterByName("RigidbodyB", p)) return;

	IRigidbody* bRB = (IRigidbody*)p.valuePtr;
	if (!bRB) return;
	if (bRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERPLANE) return;

	ColliderBox* boxCollider = (ColliderBox*)aRB->GetCollider();
	if (!boxCollider) return;

	ColliderPlane* planeCollider = (ColliderPlane*)bRB->GetCollider();
	if (!planeCollider) return;

	if (!aRB->IsFixed())
	{
		//Bounce
		Vector3D velocityVector = glm::normalize(aRB->GetVelocity());
		Vector3D planeNormal = ((ColliderPlane::Properties*)((ColliderPlane*)bRB->GetCollider())->GetTransformedProperties())->normal;

		bool findOverlap = collision.FindCollisionInfoByTag("overlapdistance", p);
		if (findOverlap)
		{
			float overlapDist = strtof(p.value.c_str(), NULL);
			Vector3D p = boxCollider->GetTransformedProperties()->GetTransform()->GetPosition();
			boxCollider->GetTransformedProperties()->GetTransform()->SetPosition(p + (overlapDist * planeNormal));
			aRB->SetPosition(aRB->GetPosition() + (overlapDist * planeNormal));
		}

		Vector3D reflectionVec = glm::reflect(velocityVector, planeNormal);
		reflectionVec = glm::normalize(reflectionVec);

		//Gets length of the speed vector
		float speed = glm::length(aRB->GetVelocity());

		//Bounce off the ball by changing the direction
		aRB->SetVelocity(reflectionVec * speed /** (float)deltaTime*/);

		//Damp speed of ball
		float damping = aRB->GetDampingRatio();
		//aRB->velocity = aRB->velocity / damping;
		aRB->SetVelocity(aRB->GetVelocity() * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
	}
}

/*Captures collisions for objects that have internal components.*/
const bool SystemPhysics::TestCollision(ISoftbody*& bSB, std::vector<Collision_s>& collisionVector)
{
	if (!bSB || !bSB->AllowCollisionDetection())
	{
		//This is not a rigidbody or does not support collisions
		return false;
	}

	if (bSB->GetTypeID() == ENGINE_OBJ_ID_PHY_SOFTBODY)
	{
		Softbody* _bSB = dynamic_cast<Softbody*>(bSB);

		std::vector<Softbody::Node*> nodes;
		if (_bSB->GetNodes(nodes))
		{
			for (Softbody::Node* nodeA : nodes)
			{
				/*Get rigidbody of node A*/
				IRigidbody* aRB = nodeA->GetComponent<IRigidbody*>();
				if (!aRB) continue;

				ICollider* aCollider = aRB->GetCollider();
				if (!aCollider) continue;

				for (Softbody::Node* nodeB : nodes)
				{
					/*Do not collide with itself*/
					if (nodeA == nodeB) continue;
					/*Do not collide with neighbors*/
					if (nodeA->IsConnectedTo(nodeB)) continue;

					/*Get rigidbody of node B*/
					IRigidbody* bRB = nodeB->GetComponent<IRigidbody*>();
					if (!bRB) continue;

					ICollider* bCollider = bRB->GetCollider();
					if (!bCollider) continue;
					
					if (bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE && aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERPLANE)
					{
						ParameterContainer paramContainer;
						ColliderSphere* sphere =(ColliderSphere*)bCollider;
						ColliderPlane* plane = (ColliderPlane*)aCollider;
						if (TestSphereToPlane(sphere, plane, paramContainer))
						{
							paramContainer.Add(Parameter("string", "RigibodyA", aRB, false));
							paramContainer.Add(Parameter("string", "RigibodyB", bRB, false));
							paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::SphereToPlane, false));
							IPhysicsComponent* aPC = aRB, * bPC = bRB;
							collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_RB));
						}

						continue;
					}
					else if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE && bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE)
					{
						ParameterContainer paramContainer;
						ColliderSphere* sphereA = (ColliderSphere*)aCollider;
						ColliderSphere* sphereB = (ColliderSphere*)bCollider;
						if (TestSphereToSphere(sphereA, sphereB, paramContainer))
						{
							paramContainer.Add(Parameter("string", "RigibodyA", aRB, false));
							paramContainer.Add(Parameter("string", "RigibodyB", bRB, false));
							paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::SphereToSphere, false));
							IPhysicsComponent* aPC = aRB, * bPC = bRB;
							collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_RB));
						}

						continue;
					}
					else if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERBOX && bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERPLANE)
					{
						ParameterContainer paramContainer;
						ColliderBox* box = (ColliderBox*)aCollider;
						ColliderPlane* plane = (ColliderPlane*)bCollider;
						if (TestBoxToPlane(box, plane, paramContainer))
						{
							paramContainer.Add(Parameter("string", "RigibodyA", aRB, false));
							paramContainer.Add(Parameter("string", "RigibodyB", bRB, false));
							paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::BoxToPlane, false));
							IPhysicsComponent* aPC = aRB, * bPC = bRB;
							collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_RB));
						}

						continue;
					}
					else
					{
						//Don't check
						continue;
					}
				}
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return collisionVector.size() != 0;
}
/*Captures collisions between rigidbodies and softbodies.*/
const bool SystemPhysics::TestCollision(IRigidbody*& aRB, ISoftbody*& bSB, std::vector<Collision_s>& collisionVector)
{
	if (!aRB || !aRB->AllowCollisionDetection())
	{
		//This is not a rigidbody or does not support collisions
		return false;
	}

	ICollider* aCollider = aRB->GetCollider();
	if (!aCollider)
	{
		return false;
	}

	if (!bSB || !bSB->AllowCollisionDetection())
	{
		//This is not a rigidbody or does not support collisions
		return false;
	}

	if (bSB->GetTypeID() == ENGINE_OBJ_ID_PHY_SOFTBODY)
	{
		Softbody* _bSB = dynamic_cast<Softbody*>(bSB);

		std::vector<Softbody::Node*> nodes;
		if (_bSB->GetNodes(nodes))
		{
			for (Softbody::Node* node : nodes)
			{
				ICollider* bCollider = node->GetComponent<IRigidbody*>()->GetCollider();
				if (!bCollider)
				{
					continue;
				}

				if (bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE && 
					aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERPLANE)
				{
					ParameterContainer paramContainer;
					ColliderSphere* sphere = (ColliderSphere*)bCollider;
					ColliderPlane* plane = (ColliderPlane*)aCollider;
					if (TestSphereToPlane(sphere, plane, paramContainer))
					{
						paramContainer.Add(Parameter("string", "RigidbodyA", aRB, false));
						paramContainer.Add(Parameter("string", "RigidbodyB", node->GetComponent<IRigidbody*>(), false));
						paramContainer.Add(Parameter("string", "NodeID", (void*)node->GetID(), false));
						paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::SphereToPlane, false));
						IPhysicsComponent* aPC = aRB, * bPC = bSB;
						collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_SB));
					}
					
					continue;
				}
				else if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE && 
						 bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE)
				{
					ParameterContainer paramContainer;
					ColliderSphere* sphereA = (ColliderSphere*)aCollider;
					ColliderSphere* sphereB = (ColliderSphere*)bCollider;
					if (TestSphereToSphere(sphereA, sphereB, paramContainer))
					{
						paramContainer.Add(Parameter("string", "RigidbodyA", aRB, false));
						paramContainer.Add(Parameter("string", "RigidbodyB", node->GetComponent<IRigidbody*>(), false));
						paramContainer.Add(Parameter("string", "NodeID", (void*)node->GetID(), false));
						paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::SphereToSphere, false));
						IPhysicsComponent* aPC = aRB, * bPC = bSB;
						collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_SB));
					}
					
					continue;
				}
				else if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERBOX && bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERPLANE)
				{
					ParameterContainer paramContainer;
					ColliderBox* box = (ColliderBox*)aCollider;
					ColliderPlane* plane = (ColliderPlane*)bCollider;
					if (TestBoxToPlane(box, plane, paramContainer))
					{
						paramContainer.Add(Parameter("string", "RigidbodyA", aRB, false));
						paramContainer.Add(Parameter("string", "RigidbodyB", node->GetComponent<IRigidbody*>(), false));
						paramContainer.Add(Parameter("string", "NodeID", (void*)node->GetID(), false));
						paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::BoxToPlane, false));
						IPhysicsComponent* aPC = aRB, * bPC = bSB;
						collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_SB));
					}

					continue;
				}
				else
				{
					//Don't check
					continue;
				}
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return collisionVector.size() != 0;
}
/*Captures collisions between rigidbodies.*/
const bool SystemPhysics::TestCollision(IRigidbody*& aRB, IRigidbody*& bRB, std::vector<Collision_s>& collisionVector)
{
	if (!aRB || !aRB->AllowCollisionDetection())
	{
		//This is not a rigidbody or does not support collisions
		return false;
	}

	ICollider* aCollider = aRB->GetCollider();
	if (!aCollider)
	{
		return false;
	}

	if (!bRB || !bRB->AllowCollisionDetection())
	{
		//This is not a rigidbody or does not support collisions
		return false;
	}

	ICollider* bCollider = bRB->GetCollider();
	if (!bCollider)
	{
		return false;
	}

	if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE && 
		bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERPLANE)
	{
		ParameterContainer paramContainer;
		ColliderSphere* sphere = (ColliderSphere*)aCollider;
		ColliderPlane* plane = (ColliderPlane*)bCollider;
		if (TestSphereToPlane(sphere, plane, paramContainer))
		{
			paramContainer.Add(Parameter("string", "RigidbodyA", aRB, false));
			paramContainer.Add(Parameter("string", "RigidbodyB", bRB, false));
			paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::SphereToPlane, false));
			IPhysicsComponent* aPC = aRB, * bPC = bRB;
			collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_RB));
			return true;
		}

		return false;
	}
	else if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE && 
			 bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE)
	{
		ParameterContainer paramContainer;
		ColliderSphere* sphereA = (ColliderSphere*)aCollider;
		ColliderSphere* sphereB = (ColliderSphere*)bCollider;
		if (TestSphereToSphere(sphereA, sphereB, paramContainer))
		{
			paramContainer.Add(Parameter("string", "RigidbodyA", aRB, false));
			paramContainer.Add(Parameter("string", "RigidbodyB", bRB, false));
			paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::SphereToSphere, false));
			IPhysicsComponent* aPC = aRB, * bPC = bRB;
			collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_RB));
			return true;
		}

		return false;
	}
	else if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE && 
			 bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERCAPSULE)
	{
		ParameterContainer paramContainer;
		ColliderSphere* sphere =(ColliderSphere*)aCollider;
		ColliderCapsule* capsule = (ColliderCapsule*)bCollider;
		if (TestSphereToCapsule(sphere, capsule, paramContainer))
		{
			paramContainer.Add(Parameter("string", "RigidbodyA", aRB, false));
			paramContainer.Add(Parameter("string", "RigidbodyB", bRB, false));
			paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::SphereToCapsule, false));
			IPhysicsComponent* aPC = aRB, * bPC = bRB;
			collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_RB));
			return true;
		}

		return false;
	}
	else if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE &&
			 bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERBOX)
	{
		ParameterContainer paramContainer;
		ColliderSphere* sphere = (ColliderSphere*)aCollider;
		ColliderBox* box = (ColliderBox*)bCollider;
		if (TestSphereToBox(sphere, box, paramContainer))
		{
			paramContainer.Add(Parameter("string", "RigidbodyA", aRB, false));
			paramContainer.Add(Parameter("string", "RigidbodyB", bRB, false));
			paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::SphereToBox, false));
			IPhysicsComponent* aPC = aRB, * bPC = bRB;
			collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_RB));
			return true;
		}

		return false;
	}
	else if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERBOX &&
			 bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERBOX)
	{
		ParameterContainer paramContainer;
		ColliderBox* boxA = (ColliderBox*)aCollider;
		ColliderBox* boxB = (ColliderBox*)bCollider;
		if (TestBoxToBox(boxA, boxB, paramContainer))
		{
			paramContainer.Add(Parameter("string", "RigidbodyA", aRB, false));
			paramContainer.Add(Parameter("string", "RigidbodyB", bRB, false));
			paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::BoxToBox, false));
			IPhysicsComponent* aPC = aRB, * bPC = bRB;
			collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_RB));
			return true;
		}

		return false;
	}
	else if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERCAPSULE && 
			 bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERCAPSULE)
	{
		ParameterContainer paramContainer;
		ColliderCapsule* capsuleA = (ColliderCapsule*)aCollider;
		ColliderCapsule* capsuleB = (ColliderCapsule*)bCollider;
		if (TestCapsuleToCapsule(capsuleA, capsuleB, paramContainer))
		{
			paramContainer.Add(Parameter("string", "RigidbodyA", aRB, false));
			paramContainer.Add(Parameter("string", "RigidbodyB", bRB, false));
			paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::CapsuleToCapsule, false));
			IPhysicsComponent* aPC = aRB, * bPC = bRB;
			collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_RB));
			return true;
		}

		return false;
	}
	else if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERCAPSULE && 
			 bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERPLANE)
	{
		ParameterContainer paramContainer;
		ColliderCapsule* capsule = (ColliderCapsule*)aCollider;
		ColliderPlane* plane = (ColliderPlane*)bCollider;
		if (TestCapsuleToPlane(capsule, plane, paramContainer))
		{
			paramContainer.Add(Parameter("string", "RigidbodyA", aRB, false));
			paramContainer.Add(Parameter("string", "RigidbodyB", bRB, false));
			paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::CapsuleToPlane, false));
			IPhysicsComponent* aPC = aRB, * bPC = bRB;
			collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_RB));
			return true;
		}

		return false;
	}
	else if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERBOX && 
			 bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERPLANE)
	{
		ParameterContainer paramContainer;
		ColliderBox* box = (ColliderBox*)aCollider;
		ColliderPlane* plane = (ColliderPlane*)bCollider;
		if (TestBoxToPlane(box, plane, paramContainer))
		{
			paramContainer.Add(Parameter("string", "RigidbodyA", aRB, false));
			paramContainer.Add(Parameter("string", "RigidbodyB", bRB, false));
			paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::BoxToPlane, false));
			IPhysicsComponent* aPC = aRB, * bPC = bRB;
			collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_RB));
			return true;
		}

		return false;
	}
	else if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERBOX &&
			 bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERBOX)
	{
		ParameterContainer paramContainer;
		ColliderBox* boxA = (ColliderBox*)aCollider;
		ColliderBox* boxB = (ColliderBox*)bCollider;
		if (TestBoxToBox(boxA, boxB, paramContainer))
		{
			paramContainer.Add(Parameter("string", "RigidbodyA", aRB, false));
			paramContainer.Add(Parameter("string", "RigidbodyB", bRB, false));
			paramContainer.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::BoxToBox, false));
			IPhysicsComponent* aPC = aRB, *bPC = bRB;
			collisionVector.push_back(Collision_s(aPC, bPC, paramContainer, COLLISION_TYPE_RB_RB));
			return true;
		}

		return false;
	}
	else
	{
		//Don't check
	}

	return false;
}
/*Captures collisions for objects that have internal components.*/
const bool SystemPhysics::TestCollision(Entity*& a, std::vector<Collision_s>& collisionVector)
{
	if (a->HasComponent<ISoftbody*>())
	{
		ISoftbody* sb = a->GetComponent<ISoftbody*>();
		return TestCollision(sb, collisionVector);
	}

	return false;
}
/*Captures collisions between gameobjects.*/
const bool SystemPhysics::TestCollision(Entity*& a, Entity*& b, std::vector<Collision_s>& collisionVector)
{
	bool rc = false;
	/*Disposing entity test*/
	if (!a || !b) return false;
	if (a->IsDisposing() || b->IsDisposing()) return false;

	if (a->HasComponent<IRigidbody*>() && b->HasComponent<IRigidbody*>())
	{
		IRigidbody* aRB = a->GetComponent<IRigidbody*>();
		IRigidbody* bRB = b->GetComponent<IRigidbody*>();
		if (!aRB->IsActive() || !bRB->IsActive())
		{
			return false;
		}

		rc = TestCollision(aRB, bRB, collisionVector);
	}
	else if (a->HasComponent<ISoftbody*>() && b->HasComponent<IRigidbody*>())
	{
		ISoftbody* aRB = a->GetComponent<ISoftbody*>();
		IRigidbody* bRB = b->GetComponent<IRigidbody*>();
		if (!aRB->IsActive() || !bRB->IsActive())
		{
			return false;
		}

		rc = TestCollision(bRB, aRB, collisionVector);
	}
	else if (b->HasComponent<ISoftbody*>() && a->HasComponent<IRigidbody*>())
	{
		ISoftbody* bRB = a->GetComponent<ISoftbody*>();
		IRigidbody* aRB = b->GetComponent<IRigidbody*>();
		if (!aRB->IsActive() || !bRB->IsActive())
		{
			return false;
		}

		rc = TestCollision(aRB, bRB, collisionVector);
	}
	else
	{
		//	¯\(°_o)/¯
	}

	return rc;
}
/*Collision response between rigidbodies - sphere-sphere and sphere-plane only.*/
void SystemPhysics::Collide_RBRB(Collision_s& collision)
{
	size_t collisionCount = collision.collisionInfo.GetCount();
	if (collisionCount == 0) return;

	bool rc = false;
	Parameter p;
	for (unsigned long i = 0; i < collisionCount; i++)
	{
		rc = collision.collisionInfo.FindParameterByIndex(i, p);
		if (p.name == "CollisionType")
		{
			CollisionType_e type = (CollisionType_e)(int)(p.valuePtr);

			switch (type)
			{
			case CollisionType_e::SphereToSphere:
			{
				if (flags & DISABLEDEFAULTRESPONSE_SphereToSphere) return;

				OnSphereToSphereCollision(collision);
			}
			break;
			case CollisionType_e::SphereToPlane:
			{
				if (flags & DISABLEDEFAULTRESPONSE_SphereToPlane) return;

				OnSphereToPlaneCollision(collision);
			}
			break;
			case CollisionType_e::SphereToCapsule:
			{
				if (flags & DISABLEDEFAULTRESPONSE_SphereToCapsule) return;

				OnSphereToCapsuleCollision(collision);
			}
			break;
			case CollisionType_e::SphereToBox:
			{
				if (flags & DISABLEDEFAULTRESPONSE_SphereToBox) return;

				OnSphereToBoxCollision(collision);
			}
			break;
			case CollisionType_e::CapsuleToCapsule:
			{
				if (flags & DISABLEDEFAULTRESPONSE_CapsuleToCapsule) return;

				OnCapsuleToCapsuleCollision(collision);
			}
			break;
			case CollisionType_e::CapsuleToPlane:
			{
				if (flags & DISABLEDEFAULTRESPONSE_CapsuleToPlane) return;

				OnCapsuleToPlaneCollision(collision);
			}
			break;
			case CollisionType_e::BoxToPlane:
			{
				if (flags & DISABLEDEFAULTRESPONSE_BoxToPlane) return;

				OnBoxToPlaneCollision(collision);
			}
			break;
			case CollisionType_e::BoxToBox:
			{
				if (flags & DISABLEDEFAULTRESPONSE_BoxToBox) return;

				OnBoxToBoxCollision(collision);
			}
			break;
			case CollisionType_e::None:
			default:
				break;
			}
		}
		else
		{
			continue;
		}
	}
}
/*Collision response between rigidbodies and softbodies - sphere-sphere and sphere-plane only.*/
void SystemPhysics::Collide_RBSB(Collision_s& collision)
{
	size_t collisionCount = collision.collisionInfo.GetCount();
	if (collisionCount == 0) return;

	bool rc = false;
	Parameter p;
	for (unsigned long i = 0; i < collisionCount; i++)
	{
		rc = collision.collisionInfo.FindParameterByIndex(i, p);
		if (p.name == "CollisionType")
		{
			CollisionType_e type = (CollisionType_e)(int)(p.valuePtr);

			switch (type)
			{
			case CollisionType_e::SphereToSphere:
			{
				IRigidbody* aRB = collision.a->GetComponent<IRigidbody*>();
				if (!aRB) return;
				if (aRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERSPHERE) return;

				ISoftbody* bSB = collision.b->GetComponent<ISoftbody*>();
				if (!bSB || bSB->GetTypeID() != ENGINE_OBJ_ID_PHY_SOFTBODY) return;

				Parameter nodeIDSTR;
				if (!collision.FindCollisionInfoByTag("NodeID", nodeIDSTR)) return;

				IRigidbody* bRB = ((Softbody*)bSB)->GetNodeRigidbodyByID((unsigned int)nodeIDSTR.valuePtr);
				if (bRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERSPHERE) return;

				ColliderSphere* sphereA = (ColliderSphere*)aRB->GetCollider();
				if (!sphereA) return;

				ColliderSphere* sphereB = (ColliderSphere*)bRB->GetCollider();
				if (!sphereB) return;

				//https://en.wikipedia.org/wiki/Inelastic_collision
				Vector3D AvelocityVector = glm::normalize(aRB->GetVelocity());
				Vector3D BvelocityVector = glm::normalize(bRB->GetVelocity());
				Vector3D AvelocityFinal = Vector3D();
				Vector3D BvelocityFinal = Vector3D();
				float c = 0.1f;
				float ma = aRB->GetMass();
				float mb = bRB->GetMass();
				Vector3D v0_A = aRB->GetVelocity();
				Vector3D v0_B = bRB->GetVelocity();
				AvelocityFinal = (c * mb * (v0_B - v0_A) + ma * v0_A + mb * v0_B) / (ma + mb);
				BvelocityFinal = (c * mb * (v0_A - v0_B) + ma * v0_A + mb * v0_B) / (ma + mb);

				bool findOverlap = collision.FindCollisionInfoByTag("overlapdistance", p);
				float overlapDistance = 1.0f;
				if (findOverlap)
				{
					overlapDistance = strtof(p.value.c_str(), NULL);
				}

				{
					//if (aRB->GetInverseMass() != INFINITE_INVERSEMASS)
					//{
					//	Vector3D p = sphereA->GetTransformedProperties()->GetTransform()->GetPosition();
					//	sphereA->GetTransformedProperties()->GetTransform()->SetPosition(p + overlapDistance * glm::normalize(bRB->GetPosition() - aRB->GetPosition()));
					//	aRB->SetPosition(aRB->GetPosition() + overlapDistance * glm::normalize(bRB->GetPosition() - aRB->GetPosition()));

					//	Vector3D reflectionVec = glm::reflect(AvelocityVector, BvelocityVector);
					//	reflectionVec = glm::normalize(reflectionVec);

					//	//Gets length of the speed vector
					//	float speed = glm::length(AvelocityFinal);

					//	//Bounce off the ball by changing the direction
					//	aRB->SetVelocity(reflectionVec * speed);

					//	//Damp speed of ball
					//	float damping = aRB->GetDampingRatio();
					//	//aRB->velocity = aRB->velocity / damping;
					//	aRB->SetVelocity(AvelocityFinal * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
					//}

					if (!bRB->IsFixed())
					{
						Vector3D p = sphereB->GetTransformedProperties()->GetTransform()->GetPosition();
						sphereB->GetTransformedProperties()->GetTransform()->SetPosition(p + overlapDistance * glm::normalize(aRB->GetPosition() - bRB->GetPosition()));
						bRB->SetPosition(bRB->GetPosition() + overlapDistance * glm::normalize(aRB->GetPosition() - bRB->GetPosition()));

						Vector3D reflectionVec = glm::reflect(BvelocityVector, AvelocityVector);
						reflectionVec = glm::normalize(reflectionVec);

						//Gets length of the speed vector
						float speed = glm::length(BvelocityFinal);

						//Bounce off the ball by changing the direction
						bRB->SetVelocity(reflectionVec * speed);

						//Damp speed of ball
						float damping = bRB->GetDampingRatio();
						//bRB->velocity = bRB->velocity / damping;
						bRB->SetVelocity(BvelocityFinal * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
					}
				}
			}
			break;
			case CollisionType_e::SphereToPlane:
			{
				IRigidbody* aRB = collision.a->GetComponent<IRigidbody*>();
				if (aRB)
				{
					if (aRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERPLANE)
					{
						return;
					}
				}
				else
				{
					return;
				}

				ISoftbody* bSB = collision.b->GetComponent<ISoftbody*>();
				if (!bSB || bSB->GetTypeID() != ENGINE_OBJ_ID_PHY_SOFTBODY)
				{
					return;
				}

				Parameter nodeIDSTR;
				if (!collision.FindCollisionInfoByTag("NodeID", nodeIDSTR))
				{
					return;
				}

				IRigidbody* bRB = ((Softbody*)bSB)->GetNodeRigidbodyByID((unsigned int)nodeIDSTR.valuePtr);
				if (bRB)
				{
					if (bRB->GetCollider()->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERSPHERE)
					{
						return;
					}
				}
				else
				{
					return;
				}

				ColliderSphere* sphereCollider = (ColliderSphere*)bRB->GetCollider();
				if (!sphereCollider)
				{
					return;
				}
				ColliderPlane* planeCollider = (ColliderPlane*)aRB->GetCollider();
				if (!planeCollider)
				{
					return;
				}

				if (!bRB->IsFixed())
				{
					//Bounce
					Vector3D velocityVector = glm::normalize(bRB->GetVelocity());
					Vector3D planeNormal = ((ColliderPlane::Properties*)((ColliderPlane*)aRB->GetCollider())->GetTransformedProperties())->normal;

					bool findOverlap = collision.FindCollisionInfoByTag("overlapdistance", p);
					if (findOverlap)
					{
						float overlapDist = strtof(p.value.c_str(), NULL);
						Vector3D p = sphereCollider->GetTransformedProperties()->GetTransform()->GetPosition();
						sphereCollider->GetTransformedProperties()->GetTransform()->SetPosition(p + (overlapDist * planeNormal));
						bRB->SetPosition(bRB->GetPosition() + (overlapDist * planeNormal));
					}

					Vector3D reflectionVec = glm::reflect(velocityVector, planeNormal);
					reflectionVec = glm::normalize(reflectionVec);

					//Gets length of the speed vector
					float speed = glm::length(bRB->GetVelocity());

					//Bounce off the ball by changing the direction
					bRB->SetVelocity(reflectionVec * speed /** (float)deltaTime*/);

					//Damp speed of ball
					float damping = bRB->GetDampingRatio();
					//aRB->velocity = aRB->velocity / damping;
					bRB->SetVelocity(bRB->GetVelocity() * (float)glm::pow(1 / damping, frameTime.GetDeltaTime()));
				}
			}
			break;
			case CollisionType_e::None:
			default:
				break;
			}
		}
		else
		{
			continue;
		}
	}
}
/*Applies the basic collision response for physics components.*/
void SystemPhysics::Collide(Collision_s& collision)
{
	if (collision.a->IsDisposing() || collision.b->IsDisposing()) return;

	if (collision.collisionType == COLLISION_TYPE_RB_RB)
	{
		Collide_RBRB(collision);
	}
	else if (collision.collisionType == COLLISION_TYPE_RB_SB)
	{
		Collide_RBSB(collision);
	}
	else
	{
		//Type does not exist
	}
}
/*Collision test between capsule and plane.*/
const bool SystemPhysics::TestCapsuleToPlane(ColliderCapsule*& capsule, ColliderPlane*& plane, ParameterContainer& collisionInfo)
{
	float cRadius = ((ColliderCapsule::Properties*)capsule->GetTransformedProperties())->radius;
	Vector3D cCenter = capsule->GetTransformedProperties()->GetTransform()->GetPosition();
	Vector3D cPA = ((ColliderCapsule::Properties*)capsule->GetTransformedProperties())->pointA;
	Vector3D cPB = ((ColliderCapsule::Properties*)capsule->GetTransformedProperties())->pointB;

	Vector3D pointInPlane = ((ColliderPlane::Properties*)plane->GetTransformedProperties())->vertices[0];
	Vector3D planeNormal = ((ColliderPlane::Properties*)plane->GetTransformedProperties())->normal;

#ifdef ENABLE_FINITE_PLANES
	//Makes the plane finite
	//This checks if the sphere is within a certain region on top of the plane - like an invisible box
	//If not, then don't check for collisions
	if (!MathHelper::IsPointWithinPlane(cPA, planeNormal, 2 * cRadius, ((ColliderPlane::Properties*)plane->GetTransformedProperties())->vertices))
	{
		return false;
	}
#endif

	//Calculate a vector from the point on the plane to the center of the sphere
	Vector3D vecTemp = cPA - pointInPlane;

	//Calculate the distance: dot product of the new vector with the plane's normal
	float distance = glm::dot(vecTemp, planeNormal);// + /*the d*/glm::dot(pointInPlane, planeNormal);
	//float distance = glm::length(vecTemp);
	if (fabs(distance) > cRadius)
	{
		//The sphere A is not touching the plane, try sphere B
#ifdef ENABLE_FINITE_PLANES
		//Makes the plane finite
		//This checks if the sphere is within a certain region on top of the plane - like an invisible box
		//If not, then don't check for collisions
		if (!MathHelper::IsPointWithinPlane(cPB, planeNormal, 2 * cRadius, ((ColliderPlane::Properties*)plane->GetTransformedProperties())->vertices))
		{
			return false;
		}
#endif

		Vector3D vecTemp = cPB - pointInPlane;
		distance = glm::dot(vecTemp, planeNormal);
		if (fabs(distance) > cRadius)
		{
			//Not Colliding
			return false;
		}
		else
		{
			collisionInfo.Add(Parameter("float", "distance", std::to_string(distance), false));
			collisionInfo.Add(Parameter("float", "overlapdistance", std::to_string(cRadius - distance), false));
			return true;
		}
	}

	collisionInfo.Add(Parameter("float", "distance", std::to_string(distance), false));
	collisionInfo.Add(Parameter("float", "overlapdistance", std::to_string(cRadius - distance), false));

	//Else, the capsule is colliding with the plane
	return true;
}
/*Collision test between capsule and capsule.*/
const bool SystemPhysics::TestCapsuleToCapsule(ColliderCapsule*& capsule0, ColliderCapsule*& capsule1, ParameterContainer& collisionInfo)
{
	float c0Radius = ((ColliderCapsule::Properties*)capsule0->GetTransformedProperties())->radius;
	Vector3D c0PA = ((ColliderCapsule::Properties*)capsule0->GetTransformedProperties())->pointA;
	Vector3D c0PB = ((ColliderCapsule::Properties*)capsule0->GetTransformedProperties())->pointB;

	float c1Radius = ((ColliderCapsule::Properties*)capsule1->GetTransformedProperties())->radius;
	Vector3D c1PA = ((ColliderCapsule::Properties*)capsule1->GetTransformedProperties())->pointA;
	Vector3D c1PB = ((ColliderCapsule::Properties*)capsule1->GetTransformedProperties())->pointB;
	
	// Compute (squared) distance between the inner structures of the capsules
	float s, t;
	Vector3D c1, c2;

	float dist2 = MathHelper::ClosestPtSegmentSegment(c0PA, c0PB, c1PA, c1PB, s, t, FLT_EPSILON, c1, c2);
		// If (squared) distance smaller than (squared) sum of radii, they collide
	float radius = c0Radius + c1Radius;

	return dist2 <= radius * radius;
}
/*Collision test between sphere and capsule.*/
const bool SystemPhysics::TestSphereToCapsule(ColliderSphere*& sphere, ColliderCapsule*& capsule, ParameterContainer& collisionInfo)
{
	float sRadius = ((ColliderSphere::Properties*)sphere->GetTransformedProperties())->radius;
	Vector3D sCenter = ((ColliderSphere::Properties*)sphere->GetTransformedProperties())->transform->GetPosition();
	float cRadius = ((ColliderCapsule::Properties*)capsule->GetTransformedProperties())->radius;
	Vector3D cPA = ((ColliderCapsule::Properties*)capsule->GetTransformedProperties())->pointA;
	Vector3D cPB = ((ColliderCapsule::Properties*)capsule->GetTransformedProperties())->pointB;

	// Compute (squared) distance between sphere center and capsule line segment
	/*Vector3D closestPoint = glm::closestPointOnLine(sCenter, cPA, cPB);
	float d = glm::distance2(closestPoint, sCenter);*/

	float dist2 = MathHelper::SqDistPointSegment(cPA, cPB, sCenter);
	// If (squared) distance smaller than (squared) sum of radii, they collide
	float radius = sRadius + cRadius;
	return dist2 <= radius * radius;
}
/*Collision test between sphere and plane.*/
const bool SystemPhysics::TestSphereToPlane(ColliderSphere*& sphere, ColliderPlane*& plane, ParameterContainer& collisionInfo)
{
	Vector3D sphereCenter = sphere->GetTransformedProperties()->GetTransform()->GetPosition();
	float sphereRadius = ((ColliderSphere::Properties*)sphere->GetTransformedProperties())->radius;
	Vector3D pointInPlane = ((ColliderPlane::Properties*)plane->GetTransformedProperties())->vertices[0];
	Vector3D planeNormal = ((ColliderPlane::Properties*)plane->GetTransformedProperties())->normal;
	Vector3D planeCenter = plane->GetTransformedProperties()->GetTransform()->GetPosition();
	//Distance = (A * x0 + B * y0 + C * z0 + D) / Sqrt(A * A + B * B + C * C)

#ifdef ENABLE_FINITE_PLANES
	/*if (!sphere->Raycast(planeCenter, planeNormal, sphereRadius * 2))
	{
		return false;
	}*/

	/*if (sphere->GetParent())
	{
		IRigidbody* sphrb = (IRigidbody*)sphere->GetParent();
		if (sphrb)
		{
			if (!plane->Raycast(sphereCenter, glm::normalize(sphrb->GetVelocity()), sphereRadius * 2))
			{
				return false;
			}
		}
	}*/

	//Makes the plane finite
	//This checks if the sphere is within a certain region on top of the plane - like an invisible box
	//If not, then don't check for collisions
	float height = sphereRadius < 1.0f ? 1.0f : 2 * sphereRadius;
	if (!MathHelper::IsPointWithinPlane(sphereCenter, planeNormal, height, ((ColliderPlane::Properties*)plane->GetTransformedProperties())->vertices))
	{
		return false;
	}
#endif

	//Calculate a vector from the point on the plane to the center of the sphere
	Vector3D vecTemp = sphereCenter - pointInPlane;

	//Calculate the distance: dot product of the new vector with the plane's normal
	float distance = glm::dot(vecTemp, planeNormal);// + /*the d*/glm::dot(pointInPlane, planeNormal);
	//float distance = glm::length(vecTemp);
	if (fabs(distance) > sphereRadius)
	{
		//The sphere is not touching the plane
		return false;
	}

	collisionInfo.Add(Parameter("float", "distance", std::to_string(distance), false));
	collisionInfo.Add(Parameter("float", "overlapdistance", std::to_string(sphereRadius - distance), false));

	//Else, the sphere is colliding with the plane
	return true;
}
/*Collision test between sphere and sphere.*/
const bool SystemPhysics::TestSphereToSphere(ColliderSphere*& sphere0, ColliderSphere*& sphere1, ParameterContainer& collisionInfo)
{
	bool rc = false;

	float s0radius = ((ColliderSphere::Properties*)sphere0->GetTransformedProperties())->radius;
	float s1radius = ((ColliderSphere::Properties*)sphere1->GetTransformedProperties())->radius;

	float distance = 0.0f;
	rc = MathHelper::TestSphereToSphere(sphere0->GetTransformedProperties()->GetTransform()->GetPosition(), s0radius, sphere1->GetTransformedProperties()->GetTransform()->GetPosition(), s1radius, distance);

	collisionInfo.Add(Parameter("float", "overlapdistance", std::to_string(distance), false));

	return rc;
}
/*Collision test between sphere and box.*/
const bool SystemPhysics::TestSphereToBox(ColliderSphere*& sphere, ColliderBox*& box, ParameterContainer& collisionInfo)
{
	float sRadius = ((ColliderSphere::Properties*)sphere->GetTransformedProperties())->radius;
	Vector3D sCenter = ((ColliderSphere::Properties*)sphere->GetTransformedProperties())->transform->GetPosition();
	Vector3D intersectionPoint = Vector3D();

	//ClosestPtPointBox(sCenter, box, intersectionPoint);
	// Sphere and OBB intersect if the (squared) distance from sphere
	// center to point p is less than the (squared) sphere radius
	
	ClosestPtPointFixedBox(sCenter, box, intersectionPoint);
	Vector3D v = intersectionPoint - sCenter;
	float sqdistance = glm::dot(v, v);
	if (sqdistance <= sRadius * sRadius)
	{
		float distance = glm::distance(intersectionPoint, sCenter);
		collisionInfo.Add(Parameter("float", "distance", std::to_string(distance), false));
		collisionInfo.Add(Parameter("float", "overlapdistance", std::to_string(sRadius - distance), false));
		collisionInfo.Add("Vector3D", "intersectionPoint", StringHelpers::Vec3ToStr(intersectionPoint));

		return true;
	}

	return false;
}
void SystemPhysics::ClosestPtPointBox(const Vector3D& sphereCenter, ColliderBox* box, Vector3D& q)
{
	//BOX
	Vector3D boxCenter = box->GetTransformedProperties()->GetTransform()->GetPosition();
	Vector3D boxMax = ((ColliderBox::Properties*)box->GetTransformedProperties())->max;
	Vector3D boxMin = ((ColliderBox::Properties*)box->GetTransformedProperties())->min;
	Vector3D d = sphereCenter - boxCenter;
	//Positive Extents
	Vector3D boxRight = Vector3D(boxMax.x, boxCenter.y, boxCenter.z);
	Vector3D boxUp = Vector3D(boxCenter.x, boxMax.y, boxCenter.z);
	Vector3D boxForward = Vector3D(boxCenter.x, boxCenter.y, boxMax.z);

	//Half lengths
	float e[3] = { glm::distance(boxUp, boxCenter), glm::distance(boxRight, boxCenter), glm::distance(boxForward, boxCenter) };
	
	//OBB axis
	Vector3D u[3] = { boxCenter - boxUp, boxCenter - boxRight, boxCenter - boxForward };

	// Start result at center of box; make steps from there
	q = boxCenter;
	// For each OBB axis...
	float dist = 0.0f;
	for (unsigned int i = 0; i < 3; i++) 
	{
		// ...project d onto that axis to get the distance
		// along the axis of d from the box center
		dist = glm::dot(d, u[i]);
		// If distance farther than the box extents, clamp to the box
		if (dist > e[i]) dist = e[i];
		if (dist < -e[i]) dist = -e[i];

		// Step that distance along the axis to get world coordinate
		q += dist * u[i];
	}
}
void SystemPhysics::ClosestPtPointFixedBox(const Vector3D& sphereCenter, ColliderBox* box, Vector3D& q/*Point p, AABB b, Point& q*/)
{
	// For each coordinate axis, if the point coordinate value is
	// outside box, clamp it to the box, else keep it as is
	for (int i = 0; i < 3; i++) 
	{
		float v = sphereCenter[i];
		if (v < ((ColliderBox::Properties*)box->GetTransformedProperties())->min[i])
		{
			v = ((ColliderBox::Properties*)box->GetTransformedProperties())->min[i]; // v = max(v, b.min[i])
		}
		if (v > ((ColliderBox::Properties*)box->GetTransformedProperties())->max[i])
		{
			v = ((ColliderBox::Properties*)box->GetTransformedProperties())->max[i]; // v = min(v, b.max[i])
		}
		q[i] = v;
	}
}
/*Collision test between capsule and box.*/
const bool SystemPhysics::TestCapsuleToBox(ColliderCapsule*& capsule, ColliderBox*& box, ParameterContainer& collisionInfo)
{
//TODO:
	return false;
}
/*Collision test between box and plane.*/
const bool SystemPhysics::TestBoxToPlane(ColliderBox*& box, ColliderPlane*& plane, ParameterContainer& collisionInfo)
{
	//PLANE
	Vector3D pointInPlane = ((ColliderPlane::Properties*)plane->GetTransformedProperties())->vertices[0];
	Vector3D planeNormal = ((ColliderPlane::Properties*)plane->GetTransformedProperties())->normal;

	//BOX
	Vector3D boxCenter = box->GetTransformedProperties()->GetTransform()->GetPosition();
	Vector3D boxMax = ((ColliderBox::Properties*)box->GetTransformedProperties())->max;
	Vector3D boxMin = ((ColliderBox::Properties*)box->GetTransformedProperties())->min;
	//Positive Extents
	Vector3D boxRight = Vector3D(boxMax.x, boxCenter.y, boxCenter.z);
	Vector3D boxUp = Vector3D(boxCenter.x, boxMax.y, boxCenter.z);
	Vector3D boxForward = Vector3D(boxCenter.x, boxCenter.y, boxMax.z);

	//Half lengths
	float e0 = glm::distance(boxUp, boxCenter);
	float e1 = glm::distance(boxRight, boxCenter);
	float e2 = glm::distance(boxForward, boxCenter);

	//OBB axis
	Vector3D u0 = boxCenter - boxUp;
	Vector3D u1 = boxCenter - boxRight;
	Vector3D u2 = boxCenter - boxForward;
			
	float r = e0 * fabs(glm::dot(planeNormal, u0)) + e1 * fabs(glm::dot(planeNormal, u1)) + e2 * fabs(glm::dot(planeNormal, u2));

#ifdef ENABLE_FINITE_PLANES
	//Makes the plane finite
	//This checks if the sphere is within a certain region on top of the plane - like an invisible box
	//If not, then don't check for collisions
	if (!MathHelper::IsPointWithinPlane(boxCenter, planeNormal, 2 * r, ((ColliderPlane::Properties*)plane->GetTransformedProperties())->vertices))
	{
		return false;
	}
#endif

	// Compute distance of box center from plane
	float s = glm::dot(planeNormal, boxCenter) -glm::dot(planeNormal, pointInPlane);
	// Intersection occurs when distance s falls within [-r,+r] interval
	if (fabs(s) <= r)
	{
		collisionInfo.Add(Parameter("float", "distance", std::to_string(s), false));
		collisionInfo.Add(Parameter("float", "overlapdistance", std::to_string(r - s), false));

		return true;
	}

	return false;
}
/*Collision test between box and box.*/
const bool SystemPhysics::TestBoxToBox(ColliderBox*& box0, ColliderBox*& box1, ParameterContainer& collisionInfo)
{
	//BOX A
	Vector3D boxACenter = box0->GetTransformedProperties()->GetTransform()->GetPosition();
	Vector3D boxAMax = ((ColliderBox::Properties*)box0->GetTransformedProperties())->max;
	Vector3D boxAMin = ((ColliderBox::Properties*)box0->GetTransformedProperties())->min;
	//Positive Extents
	Vector3D boxARight = Vector3D(boxAMax.x, boxACenter.y, boxACenter.z);
	Vector3D boxAUp = Vector3D(boxACenter.x, boxAMax.y, boxACenter.z);
	Vector3D boxAForward = Vector3D(boxACenter.x, boxACenter.y, boxAMax.z);
	//Half lengths
	float eA[3] = { glm::distance(boxAUp, boxACenter), glm::distance(boxARight, boxACenter), glm::distance(boxAForward, boxACenter) };
	//OBB axis
	Vector3D uA[3] = { boxACenter - boxAUp, boxACenter - boxARight, boxACenter - boxAForward };

	//BOX B
	Vector3D boxBCenter = box1->GetTransformedProperties()->GetTransform()->GetPosition();
	Vector3D boxBMax = ((ColliderBox::Properties*)box1->GetTransformedProperties())->max;
	Vector3D boxBMin = ((ColliderBox::Properties*)box1->GetTransformedProperties())->min;
	//Positive Extents
	Vector3D boxBRight = Vector3D(boxBMax.x, boxBCenter.y, boxBCenter.z);
	Vector3D boxBUp = Vector3D(boxBCenter.x, boxBMax.y, boxBCenter.z);
	Vector3D boxBForward = Vector3D(boxBCenter.x, boxBCenter.y, boxBMax.z);
	//Half lengths
	float eB[3] = { glm::distance(boxBUp, boxBCenter), glm::distance(boxBRight, boxBCenter), glm::distance(boxBForward, boxBCenter) };
	//OBB axis
	Vector3D uB[3] = { boxBCenter - boxBUp, boxBCenter - boxBRight, boxBCenter - boxBForward };

	float ra, rb;
	Matrix3D R, AbsR;

	// Compute rotation matrix expressing b in a's coordinate frame
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			R[i][j] = glm::dot(uA[i], uB[j]);

	// Compute translation vector t
	Vector3D t = boxBCenter - boxACenter;
	// Bring translation into a's coordinate frame
	t = Vector3D(glm::dot(t, uA[0]), glm::dot(t, uA[2]), glm::dot(t, uA[2]));

	// Compute common subexpressions. Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null (see text for details)
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			AbsR[i][j] = fabs(R[i][j]) + FLT_EPSILON;

	// Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++)
	{
		ra = eA[i];
		rb = eB[0] * AbsR[i][0] + eB[1] * AbsR[i][1] + eB[2] * AbsR[i][2];
		if (fabs(t[i]) > ra + rb) return 0;
	}

	// Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++)
	{
		ra = eA[0] * AbsR[0][i] + eA[1] * AbsR[1][i] + eA[2] * AbsR[2][i];
		rb = eB[i];
		if (fabs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return 0;
	}

	// Test axis L = A0 x B0
	ra = eA[1] * AbsR[2][0] + eA[2] * AbsR[1][0];
	rb = eB[1] * AbsR[0][2] + eB[2] * AbsR[0][1];
	if (fabs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return 0;

	// Test axis L = A0 x B1
	ra = eA[1] * AbsR[2][1] + eA[2] * AbsR[1][1];
	rb = eB[0] * AbsR[0][2] + eB[2] * AbsR[0][0];
	if (fabs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) return 0;

	// Test axis L = A0 x B2
	ra = eA[1] * AbsR[2][2] + eA[2] * AbsR[1][2];
	rb = eB[0] * AbsR[0][1] + eB[1] * AbsR[0][0];
	if (fabs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return 0;

	// Test axis L = A1 x B0
	ra = eA[0] * AbsR[2][0] + eA[2] * AbsR[0][0];
	rb = eB[1] * AbsR[1][2] + eB[2] * AbsR[1][1];
	if (fabs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return 0;

	// Test axis L = A1 x B1
	ra = eA[0] * AbsR[2][1] + eA[2] * AbsR[0][1];
	rb = eB[0] * AbsR[1][2] + eB[2] * AbsR[1][0];
	if (fabs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return 0;

	// Test axis L = A1 x B2
	ra = eA[0] * AbsR[2][2] + eA[2] * AbsR[0][2];
	rb = eB[0] * AbsR[1][1] + eB[1] * AbsR[1][0];
	if (fabs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return 0;

	// Test axis L = A2 x B0
	ra = eA[0] * AbsR[1][0] + eA[1] * AbsR[0][0];
	rb = eB[1] * AbsR[2][2] + eB[2] * AbsR[2][1];
	if (fabs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return 0;

	// Test axis L = A2 x B1
	ra = eA[0] * AbsR[1][1] + eA[1] * AbsR[0][1];
	rb = eB[0] * AbsR[2][2] + eB[2] * AbsR[2][0];
	if (fabs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return 0;

	// Test axis L = A2 x B2
	ra = eA[0] * AbsR[1][2] + eA[1] * AbsR[0][2];
	rb = eB[0] * AbsR[2][1] + eB[1] * AbsR[2][0];
	if (fabs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return 0;

	// Since no separating axis found, the OBBs must be intersecting
	return 1;
}
/*Collision test between mesh and mesh.*/
const bool SystemPhysics::TestMeshToMesh(Entity*& a, Entity*& b)
{
	//TODO:
	return false;
}
/*Contructor.*/
SystemPhysics::SystemPhysics()
{
	this->isActive = true;
	this->wind = new Wind();
	this->wind->Activate(false);
	this->airFriction = 0.0f;
	this->scene = nullptr;
	this->gravity = Vector3D();
	this->colliderFactory = new ColliderFactory();
	this->physicsComponentFactory = new PhysicsComponentFactory();
	/*this->rigidbodyFactory = new RigidbodyFactory();*/
}
/*Destructor.*/
SystemPhysics::~SystemPhysics()
{
	Dispose();


}
/*Checks if the update for the current fram was completed.*/
const bool SystemPhysics::IsUpdateComplete()
{
	return isUpdateComplete.load();
}
/*Toggles the system ON/OFF*/
void SystemPhysics::Activate(const bool& value)
{
	this->isActive.store(value);
}
/*Checks if the system is activated or not.*/
const bool SystemPhysics::IsActive()
{
	return this->isActive.load();
}

/*Tests two colliders (bounding boxes) for collisions. This uses the transformed vertices. Legacy.*/
bool SystemPhysics::IsAColliderCollision(ICollider* from, ICollider* to)
{
	bool rc = false;

	std::vector<Vector3D> vertices;
	if (!from->GetTransformedProperties()->GetExtremes(vertices)) return false;

	if (to->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE)
	{
		ColliderSphere::Properties* properties = (ColliderSphere::Properties*) to->GetTransformedProperties();

		for (unsigned int i = 0; i < vertices.size(); i++)
		{
			Vector3D vertex = vertices[i];

			Vector3D v = vertex - properties->transform->GetPosition();
			rc = (glm::dot(v, v) <= properties->radius * properties->radius);
			if (rc)
			{
				break;
			}
		}
	}
	else if (to->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERAABB)
	{
		ColliderBox::Properties* properties = (ColliderBox::Properties*) to->GetTransformedProperties();
		for (unsigned int i = 0; i < vertices.size(); i++)
		{
			Vector3D vertex = vertices[i];

			if (vertex.x > properties->min.x&& vertex.x < properties->max.x &&
				vertex.y > properties->min.y&& vertex.y < properties->max.y &&
				vertex.z > properties->min.z&& vertex.z < properties->max.z)
			{
				rc = true;
				break;
			}
		}
	}
	else if(to->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERPLANE)
	{

	}
	else if (to->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERBOX)
	{
		ColliderBox::Properties* properties = (ColliderBox::Properties*) to->GetTransformedProperties();
		for (unsigned int i = 0; i < vertices.size(); i++)
		{
			Vector3D vertex = vertices[i];

			if (vertex.x > properties->min.x&& vertex.x < properties->max.x &&
				vertex.y > properties->min.y&& vertex.y < properties->max.y &&
				vertex.z > properties->min.z&& vertex.z < properties->max.z)
			{
				rc = true;
				break;
			}
		}
	}
	/*else if (colliderType == "Capsule")
	{
		go->collisionData = new ColliderCapsule();
		memcpy(go->collisionData, this->collisionData, sizeof(ColliderCapsule));
	}*/

	return rc;
}
/*Checks if a point in inside a collider.*/
bool SystemPhysics::IsPointInCollider(const Vector3D& p, ICollider* collider)
{
	bool rc = false;

	if (collider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE)
	{
		ColliderSphere::Properties* properties = (ColliderSphere::Properties*)collider->GetTransformedProperties();

		Vector3D v = p - properties->transform->GetPosition();
		rc = (glm::dot(v, v) <= properties->radius * properties->radius);
	}
	else if (collider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERAABB)
	{
		ColliderBox::Properties* properties = (ColliderBox::Properties*)collider->GetTransformedProperties();

		rc = p.x > properties->min.x&& p.x < properties->max.x &&
			p.y > properties->min.y&& p.y < properties->max.y &&
			p.z > properties->min.z&& p.z < properties->max.z;
	}
	else if (collider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERPLANE)
	{

	}
	else if (collider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERBOX)
	{
		ColliderBox::Properties* properties = (ColliderBox::Properties*)collider->GetTransformedProperties();

		rc = p.x > properties->min.x&& p.x < properties->max.x &&
			p.y > properties->min.y&& p.y < properties->max.y &&
			p.z > properties->min.z&& p.z < properties->max.z;
	}
	/*else if (colliderType == "Capsule")
	{
		go->collisionData = new ColliderCapsule();
		memcpy(go->collisionData, this->collisionData, sizeof(ColliderCapsule));
	}*/

	return rc;
}
/*LEGACY. Tests collisions between two colliders.*/
const bool SystemPhysics::TestColliders(Entity*& a, Entity*& b, ParameterContainer& collisionInfo)
{
	IRigidbody* aRB = a->GetComponent<IRigidbody*>();
	if (!aRB || !aRB->AllowCollisionDetection())
	{
		//This is not a rigidbody or does not support collisions
		return false;
	}

	ICollider* aCollider = aRB->GetCollider();
	if (!aCollider)
	{
		return false;
	}

	IRigidbody* bRB = b->GetComponent<IRigidbody*>();
	if (!bRB || !bRB->AllowCollisionDetection())
	{
		//This is not a rigidbody or does not support collisions
		return false;
	}

	ICollider* bCollider = bRB->GetCollider();
	if (!bCollider)
	{
		return false;
	}
	
	if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE && 
		bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERPLANE)
	{
		ColliderSphere* sphere = (ColliderSphere*)aCollider;
		ColliderPlane* plane = (ColliderPlane*)bCollider;
		bool rc = TestSphereToPlane(sphere, plane, collisionInfo);
		if (rc)
		{
			collisionInfo.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::SphereToPlane, false));
		}

		return rc;
	}
	else if (aCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE && 
			 bCollider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE)
	{
		ColliderSphere* sphereA = (ColliderSphere*)aCollider;
		ColliderSphere* sphereB = (ColliderSphere*)bCollider;
		bool rc = TestSphereToSphere(sphereA, sphereB, collisionInfo);
		if (rc)
		{
			collisionInfo.Add(Parameter("string", "CollisionType", (void*)CollisionType_e::SphereToSphere, false));
		}

		return rc;
	}
	else
	{
		//Don't check
	}

	return false;
}
/*Checks if the collision is in the vector of collisions.*/
const bool SystemPhysics::IsCollisionRegistered(const Collision_s& collision)
{
	for (const Collision_s& c : collisions)
	{
		if (c.a == collision.a && c.b == collision.b)
		{
			return true;
		}

		if (c.b == collision.a && c.a == collision.b)
		{
			return true;
		}
	}

	return false;
}

/*Find the closest triangle to a position*/
void SystemPhysics::GetClosestTriangleToPosition(const Vector3D& position, Entity& gameObject1, Vector3D& closestPoint, Triangle3D& closestTriangle)
{
	// Assume the closest distance is REALLY far away
	float closestDistanceSoFar = FLT_MAX;

	std::vector<Triangle3D> triangles;
	std::vector<Vertex3D> vertices;

	std::vector<IMeshComponent*> mCs;
	if (gameObject1.GetComponents(mCs))
	{
		for (IMeshComponent* mc : mCs)
		{
			std::vector<IMesh*> meshes;
			if (mc->GetMeshes(meshes))
			{
				for (IMesh* mesh : meshes)
				{
					if (mesh->GetTransformedMesh(gameObject1.GetComponent<IRigidbody*>()->GetWorldMatrix(), mc->AllowMeshTranformations(), triangles, vertices))
					{
						for (Triangle3D triangle : triangles)
						{
							Vector3D curClosetPoint = MathHelper::ClosestPtPointTriangle(position, triangle.pointA.GetVec3(), triangle.pointB.GetVec3(), triangle.pointC.GetVec3());

							// Is this the closest so far?
							float distanceNow = glm::distance(curClosetPoint, position);

							// is this closer than the closest distance
							if (distanceNow <= closestDistanceSoFar)
							{
								closestDistanceSoFar = distanceNow;

								closestPoint = curClosetPoint;

								closestTriangle = triangle;
							}
						}

						vertices.clear();
						vertices.shrink_to_fit();
						triangles.clear();
						triangles.shrink_to_fit();
					}
				}
			}
		}
	}
}
/*Allow clients to configure and customize the current instance of the physics system.*/
void SystemPhysics::SetFlags(const unsigned long long& flags)
{
	this->flags = flags;
}
/*Integrates a rigid body.*/
void SystemPhysics::IntegrationStep(IRigidbody*& rigidbody)
{
	if (rigidbody)
	{
		/*Integrates the rigidbody if the rigidbody is NOT fixed.*/
		if(!rigidbody->IsFixed())
		{
			std::vector<double> dampening;
			dampening.push_back(/*air friction*/ glm::pow(GetAirFriction(), this->frameTime.GetDeltaTime()));
			Integrator::SemiEulerIntegration(rigidbody, (float)this->frameTime.GetDeltaTime(), GetGravity(), dampening);
		}

		/*Updates the rigibody regardless of the integration. This will transform the colliders.*/
		rigidbody->Update(this->frameTime.GetDeltaTime());
	}
}
/*Integrates a softbody.*/
void SystemPhysics::IntegrationStep(ISoftbody*& softbody)
{
	if (softbody)
	{
		if (softbody->GetTypeID() == ENGINE_OBJ_ID_PHY_SOFTBODY)
		{
			softbody->Update(this->frameTime);

			/*Every node must have a rigidbody, so this gets the rigidbodies from the nodes and integrate them one by one.*/
			std::vector<Softbody::Node*> nodes;
			if (((Softbody*)softbody)->GetNodes(nodes))
			{
				for (Softbody::Node*& node : nodes)
				{
					IRigidbody* rb = node->GetComponent<IRigidbody*>();
					IntegrationStep(rb);
				}
			}
		}
	}
}
/*Applies wind to a softbody.*/
void SystemPhysics::ApplyWind(ISoftbody*& softbody)
{
	if (!wind || !wind->IsActive()) return;
	if (!softbody) return;

	float wRadius;
	Vector3D wPosition;
	bool hasContactArea = wind->GetContactArea(wRadius, wPosition);

	if (softbody->GetTypeID() == ENGINE_OBJ_ID_PHY_SOFTBODY)
	{
		Softbody* sb = (Softbody*)softbody;

		std::vector<Softbody::Node*> nodes;
		if (sb->GetNodes(nodes))
		{
			for (auto node : nodes)
			{
				IRigidbody* rb = node->GetComponent<IRigidbody*>();
				if (rb && !rb->IsFixed())
				{
					Vector3D windForce = wind->GetForce();

					if (hasContactArea)
					{
						//Checks if the rigidbody is inside the wind contact area
						Vector3D v = rb->GetPosition() - wPosition;
						if (glm::dot(v, v) <= wRadius * wRadius)
						{
							/*Applies windforce*/	
							rb->ApplyForce(windForce);
						}
					}
					else
					{
						//Apply wind without area
						/*Applies windforce*/
						rb->ApplyForce(windForce);
					}					
				}
			}
		}
	}
}
/*Applies the wind to a rigidbody;*/
void SystemPhysics::ApplyWind(IRigidbody*& rigidbody)
{
	if (!wind) return;

	float wRadius;
	Vector3D wPosition;
	bool hasContactArea = wind->GetContactArea(wRadius, wPosition);

	if (rigidbody && !rigidbody->IsFixed())
	{
		Vector3D windForce = wind->GetForce();

		if (hasContactArea)
		{
			//Checks if the rigidbody is inside the wind contact area
			Vector3D v = rigidbody->GetPosition() - wPosition;
			if (glm::dot(v, v) <= wRadius * wRadius)
			{
				/*Applies windforce*/				
				rigidbody->ApplyForce(windForce);
			}
		}
		else
		{
			//Apply wind without area
			rigidbody->ApplyForce(windForce);
		}
	}
}
/*Toggles the wind in the system.*/
void SystemPhysics::ToggleWind(const bool& value)
{
	if (wind)
	{
		wind->Activate(value);
	}
}
/*Checks if the wind is active in the system.*/
const bool SystemPhysics::IsWindActive()
{
	if (!wind) return false;

	return wind->IsActive();
}
/*Sets the properties of the wind.*/
void SystemPhysics::SetWind(const float& radius, const Vector3D& position, const float& maxWindStep, const float& minWindStep)
{
	if (!wind) return;

	wind->SetWindData(radius, position, maxWindStep, minWindStep);
}
/*Gets the properties of the wind.*/
const bool SystemPhysics::GetWind(float& radius, Vector3D& position, float& maxWindStep, float& minWindStep, Vector3D& currentForce)
{
	if (wind)
	{
		wind->GetContactArea(radius, position);
		wind->GetWindStep(maxWindStep, minWindStep);
		currentForce = wind->GetForce();

		return true;
	}

	return false;
}
/*Sets the current air friction of the system.*/
void SystemPhysics::SetAirFriction(const float& airFriction)
{
	this->airFriction = airFriction;
}
/*Gets the air friction of the system.*/
const float SystemPhysics::GetAirFriction()
{
	return airFriction;
}
/*Sets the current gravity of the system.*/
void SystemPhysics::SetGravity(const Vector3D& gravity)
{
	this->gravity = gravity;
}
/*Gets the current gravity in the system.*/
const Vector3D SystemPhysics::GetGravity()
{
	return this->gravity;
}
/*Gets a factory of colliders.*/
IFactory<ICollider>* SystemPhysics::GetColliderFactory()
{
	if (!colliderFactory)
	{
		colliderFactory = new ColliderFactory();
	}

	return colliderFactory;
}
/*Gets a physics components factory.*/
IFactory<IPhysicsComponent>* SystemPhysics::GetPhysicsComponentFactory()
{
	return physicsComponentFactory;
}
/*Binds a collision listener for clients of this system.*/
void SystemPhysics::BindCollisionCallback(std::function<void(Collision_s&)> fn)
{
	this->collisionCallback = fn;
}
/*Sets the current scene being processed.*/
void SystemPhysics::SetScene(Scene*& scene)
{
	this->scene = scene;
	SetGravity(scene->GetGravity());
	SetAirFriction(scene->GetAirFriction());

	/*Wind Data*/
	{
		float windRadius, maxWindStep, minWindStep;
		bool isWindEnabled = false;
		Vector3D windPosition;
		scene->GetWindData(windRadius, windPosition, maxWindStep, minWindStep, isWindEnabled);
		wind->SetWindData(windRadius, windPosition, maxWindStep, minWindStep);
		/*wind->Activate(isWindEnabled);*/
	}
}
/*Updates all the collisions in the current scene.*/
void SystemPhysics::Update(const FrameTime& frameTime)
{
	if (!this->isActive.load()) return;
	isUpdateComplete.store(false);

	SetFrameTime(frameTime);

	//Updates wind
	if (wind->IsActive())
	{
		wind->Update(frameTime);
	}

	std::vector<Entity*>* entitiesPtr = scene->GetEntities();
	if (entitiesPtr)
	{
		std::vector<Entity*> entities = *entitiesPtr;
		//Integrate
		for(const auto& entity : entities)
		{
			if (entity->IsDisposing()) continue;
						
			if (entity->HasComponent<IRigidbody*>())
			{
				IRigidbody* rb = entity->GetComponent<IRigidbody*>();

				IRender* vr = entity->GetComponent<IRender*>();
				if (/*Not visible*/(vr && !vr->IsVisible()) || /*Skybox*/entity->GetTypeID() == ENGINE_OBJ_ID_SCN_SKYBOX)
				{
					/*Update the rigidbody, but do not integrate it.*/
					rb->Update(this->frameTime.GetDeltaTime());
					continue;
				}
								
				if (rb->IsActive())
				{
					//Integrates
					IntegrationStep(rb);
				}
			}

			else if (entity->HasComponent<ISoftbody*>())
			{
				ISoftbody* sb = entity->GetComponent<ISoftbody*>();
				if (sb->IsActive())
				{
					ApplyWind(sb);
					IntegrationStep(sb);
				}	
			}	
		}

		//Acquire collisions
		std::vector<Collision_s> collisionsVector;
		for (auto& a : entities)
		{
			if (a->IsDisposing()) continue;

			/*Don't test skyboxes for collisions.*/
			if (a->GetTypeID() == ENGINE_OBJ_ID_SCN_SKYBOX)	continue;

	#ifdef ENABLE_SOFTBODY_INTERNAL_COLLISION
			/*Test internal collisions*/
			if (TestCollision(a, collisionsVector))
			{
				/*Copy the collisions to the global collision vector.*/
				for (auto& col : collisionsVector)
				{
					/*Checks if this is not a double collision.*/
					if (!IsCollisionRegistered(col) && !col.IsDisposing())
					{
						//Only care about new collisions
						collisions.push_back(col);
						//Fire collision event
						if (collisionCallback)
						{
							collisionCallback(col);
						}
					}
				}

				collisionsVector.clear();
			}
	#endif

			//Acquire collisions
			for (auto& b : entities)
			{
				if (a->IsDisposing()) continue;
				if (b->IsDisposing()) continue;

				//An object cannot collide with itself
				if (a->GetUniqueID() == b->GetUniqueID()) continue;
				/*Don't test skyboxes for collisions.*/
				if (b->GetTypeID() == ENGINE_OBJ_ID_SCN_SKYBOX) continue;
						
				if (TestCollision(a, b, collisionsVector))
				{
					/*Copy the collisions to the global collision vector.*/
					for (auto& col : collisionsVector)
					{
						/*Checks if this is not a double collision or is a collision against a disposed object.*/
						if (!IsCollisionRegistered(col) && !col.IsDisposing())
						{
							//Only care about new collisions
							collisions.push_back(col);
							//Fire collision event
							if (collisionCallback)
							{
								collisionCallback(col);
							}
						}
					}
					collisionsVector.clear();
				}
			}
		}

		//Handle collisions
		for (auto& collision : collisions)
		{
			Collide(collision);
		}
		collisions.clear();
		collisions.shrink_to_fit();
	}
	
	isUpdateComplete.store(true);
}
/*Cleans up the system, frees allocated memory.*/
void SystemPhysics::Dispose()
{
	DisposePointer(wind);
	DisposePointer(colliderFactory);
	DisposePointer(physicsComponentFactory);	
}
