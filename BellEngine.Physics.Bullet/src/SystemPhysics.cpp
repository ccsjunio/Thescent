#include "SystemPhysics.h"
#include <MathHelper.h>
#include "Convert.h"
#include "IBulletRigidBodyWrapper.h"
#include "ColliderFactory.h"
#include "RigidbodyFactory.h"
#include "PhysicsComponentFactory.h"
#include <IMeshComponent.h>

SystemPhysics::SystemPhysics()
{
	this->isActive = true;
	this->scene = nullptr;
	this->airFriction = 0.0f;
	this->colliderFactory = new ColliderFactory();
	this->physicsComponentFactory = new PhysicsComponentFactory();
	/*this->rigidbodyFactory = new RigidbodyFactory();	*/
}

SystemPhysics::~SystemPhysics()
{
	Dispose();
}

void SystemPhysics::Activate(const bool& value)
{
	this->isActive.store(value);
}

const bool SystemPhysics::IsUpdateComplete()
{
	return isUpdateComplete.load();
}
const bool SystemPhysics::IsActive()
{
	return this->isActive.load();
}

void SystemPhysics::InitializeBulletWorld()
{
	BulletWorldInfo bulletWorldInfo;

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	bulletWorldInfo.collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	bulletWorldInfo.dispatcher = new btCollisionDispatcher(bulletWorldInfo.collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	bulletWorldInfo.overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	bulletWorldInfo.solver = new btSequentialImpulseConstraintSolver;

	bulletWorldInfo.dynamicsWorld = new btDiscreteDynamicsWorld(bulletWorldInfo.dispatcher, bulletWorldInfo.overlappingPairCache, bulletWorldInfo.solver, bulletWorldInfo.collisionConfiguration);

	std::vector<Entity*> entities;
	if (scene->GetEntities(entities))
	{
		for (Entity*& entity : entities)
		{
			IRigidbody* rb = entity->GetComponent<IRigidbody*>();
			if (rb)
			{
				if (rb->AllowCollisionDetection())
				{
					IBulletRigidBodyWrapper* rbWrapper = dynamic_cast<IBulletRigidBodyWrapper*>(rb);
					if (rbWrapper)
					{
						bulletWorldInfo.dynamicsWorld->addRigidBody(rbWrapper->GetWrappedRigidBody());
					}
				}
			}
		}
	}
	

	//Puts the world in a map of worlds
	worldsPerScene[scene->GetName()] = bulletWorldInfo;
}

bool SystemPhysics::TestCollision(Entity* a, Entity* b, std::vector<Parameter>& collisionInfo)
{
	return false;
}
void SystemPhysics::Collide(Collision_s& collision)
{

}

void SystemPhysics::BindCollisionCallback(std::function<void(Collision_s&)> fn)
{
	this->collisionCallback = fn;
}

void SystemPhysics::SetScene(Scene*& scene)
{	
	//Sets the scene
	this->scene = scene;

	//Creates one bullet world per scene
	auto it = worldsPerScene.find(scene->GetName());
	if (it == worldsPerScene.end())
	{
		InitializeBulletWorld();
	}
	
	SetGravity(scene->GetGravity());
	SetAirFriction(scene->GetAirFriction());
}

/*Tests two colliders (bounding boxes) for collisions. This uses the transformed vertices*/
bool SystemPhysics::IsAColliderCollision(ICollider* from, ICollider* to)
{
	bool rc = false;

	std::vector<Vector3D> vertices;
	if (!from->GetTransformedProperties()->GetExtremes(vertices)) return false;

	std::string colliderTypeFrom = from->GetType();
	std::string colliderTypeTo = to->GetType();

	if (colliderTypeTo == COLLIDER_SPHERE)
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
	else if (colliderTypeTo == COLLIDER_AABB)
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
	else if (colliderTypeTo == COLLIDER_PLANE)
	{

	}
	else if (colliderTypeTo == COLLIDER_BOX)
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

bool SystemPhysics::IsPointInCollider(const Vector3D& p, ICollider* collider)
{
	bool rc = false;

	std::string colliderType = collider->GetType();

	if (colliderType == COLLIDER_SPHERE)
	{
		ColliderSphere::Properties* properties = (ColliderSphere::Properties*)collider->GetTransformedProperties();

		Vector3D v = p - properties->transform->GetPosition();
		rc = (glm::dot(v, v) <= properties->radius * properties->radius);
	}
	else if (colliderType == COLLIDER_AABB)
	{
		ColliderBox::Properties* properties = (ColliderBox::Properties*)collider->GetTransformedProperties();

		rc = p.x > properties->min.x&& p.x < properties->max.x &&
			p.y > properties->min.y&& p.y < properties->max.y &&
			p.z > properties->min.z&& p.z < properties->max.z;
	}
	else if (colliderType == COLLIDER_PLANE)
	{

	}
	else if (colliderType == COLLIDER_BOX)
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

const bool SystemPhysics::IsWindActive()
{
	return false;
}

void SystemPhysics::ToggleWind(const bool& value)
{
}

void SystemPhysics::SetWind(const float& radius, const Vector3D& position, const float& maxWindStep, const float& minWindStep)
{
}

const bool SystemPhysics::GetWind(float& radius, Vector3D& position, float& maxWindStep, float& minWindStep, Vector3D& currentForce)
{
	return false;
}

void SystemPhysics::IntegrationStep(Entity* go, const float& deltaTime)
{
	if (!go)
	{
		return;
	}

	IRigidbody* rb = go->GetComponent<IRigidbody*>();
	if (rb)
	{
		if (rb->GetInverseMass() != INFINITE_INVERSEMASS)
		{
			//https://gafferongames.com/post/integration_basics/
			Vector3D velocity = /*Integration*/ (rb->GetVelocity() + rb->GetAcceleration() * deltaTime);
			velocity *= /*air friction*/ glm::pow(airFriction, deltaTime);
			rb->SetVelocity(velocity);
			rb->SetPosition(rb->GetPosition() + rb->GetVelocity() * deltaTime);
		}
	}
}

void SystemPhysics::SetAirFriction(const float& airFriction)
{
	this->airFriction = airFriction;
}

const float SystemPhysics::GetAirFriction()
{
	return airFriction;
}

void SystemPhysics::SetGravity(const Vector3D& gravity)
{
	auto it = worldsPerScene.find(scene->GetName());
	if (it == worldsPerScene.end())
	{
		it->second.dynamicsWorld->setGravity(Convert::Vec3ToBtVec3(gravity));
	}
}

const Vector3D SystemPhysics::GetGravity()
{
	auto it = worldsPerScene.find(scene->GetName());
	if (it == worldsPerScene.end())
	{
		return Convert::BtVec3ToVec3(it->second.dynamicsWorld->getGravity());
	}
	else
	{
		return Vector3D();
	}
}

IFactory<IPhysicsComponent>* SystemPhysics::GetPhysicsComponentFactory()
{
	return physicsComponentFactory;
}

IFactory<ICollider>* SystemPhysics::GetColliderFactory()
{
	if (!colliderFactory)
	{
		colliderFactory = new ColliderFactory();
	}

	return colliderFactory;
}

void SystemPhysics::SetFlags(const unsigned long long& flags)
{
	this->flags = flags;
}

void SystemPhysics::Dispose()
{
	for (std::pair<std::string, BulletWorldInfo> element : worldsPerScene)
	{
		int nCollisionObjects = element.second.dynamicsWorld->getNumCollisionObjects();
		for (int i = nCollisionObjects - 1; i >= 0; i--)
		{
			btCollisionObject* obj = element.second.dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				auto mSt = body->getMotionState();
				DisposePointer(mSt);
			}
			element.second.dynamicsWorld->removeCollisionObject(obj);
			DisposePointer(obj);
		}

		DisposePointer(element.second.dynamicsWorld);
		DisposePointer(element.second.solver);
		DisposePointer(element.second.overlappingPairCache);
		DisposePointer(element.second.dispatcher);
		DisposePointer(element.second.collisionConfiguration);
	}
	worldsPerScene.clear();
	
	DisposePointer(colliderFactory);
	DisposePointer(physicsComponentFactory);

	/*if (rigidbodyFactory)
	{
		delete (rigidbodyFactory);
		rigidbodyFactory = nullptr;
	}*/
}

void SystemPhysics::Update(const FrameTime& frameTime)
{
	if (!this->isActive.load()) return;
	isUpdateComplete.store(false);

	SetFrameTime(frameTime);

	//Updates bullet world
	auto it = worldsPerScene.find(scene->GetName());
	if (it != worldsPerScene.end())
	{
		int rc = it->second.dynamicsWorld->stepSimulation(this->frameTime.GetDeltaTime(), 10);

		//Updates rigidbodies
		std::vector<Entity*> entities;
		if (scene->GetEntities(entities))
		{
			for (Entity*& entity : entities)
			{
				IRigidbody* rb = entity->GetComponent<IRigidbody*>();
				if (rb)
				{
					if (rb->AllowCollisionDetection() && !rb->IsFixed())
					{
						rb->Update(this->frameTime.GetDeltaTime());
					}
				}
			}
		}		
	}

	isUpdateComplete.store(true);
}
