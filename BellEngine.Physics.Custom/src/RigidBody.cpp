/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#include "RigidBody.h"
#include <Transform.h>
#include <BellEngineID.h>
#include <math.h>

RigidBody::RigidBody()
{
	allowCollisionDetection = true;
	collider = nullptr;
	isActive = true;
	elasticity = mass = inverseMass = 0.0f;	
	velocity = acceleration = Vector3D(0.0f, 0.0f, 0.0f);
	SetTransform(new ::Transform());
}

RigidBody::~RigidBody()
{
	Dispose();	
}

ICloneable* RigidBody::Clone()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	//this->Transform();

	RigidBody* clone = new RigidBody();	
	clone->isActive.store(this->isActive.load());
	clone->acceleration = this->acceleration;
	clone->velocity = this->velocity;
	clone->inverseMass = this->inverseMass;
	clone->elasticity = this->elasticity;
	clone->mass = this->mass;
	clone->allowCollisionDetection.store(this->allowCollisionDetection.load());	
	if (this->collider)
	{
		clone->collider = (ICollider*)this->collider->Clone();
		clone->collider->SetParent(clone);
	}
	if (this->transform)
	{
		clone->transform = (ITransform*)this->transform->Clone();
	}
	clone->Transform();

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return clone;
}

const bool RigidBody::IsActive()
{
	return this->isActive.load();
}

void RigidBody::Activate(const bool& value)
{
	this->isActive.store(value);
}

void RigidBody::SetCollider(ICollider* collider)
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	this->collider = collider;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif	
}

ICollider* RigidBody::GetCollider()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	ICollider* col = this->collider;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return col;
}

void RigidBody::ToggleCollisionDetection(const bool& value)
{
	this->allowCollisionDetection.store(value);
}

const bool RigidBody::AllowCollisionDetection()
{
	return this->allowCollisionDetection.load();
}

void RigidBody::ClampVelocity()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	velocity = Vector3D(0.0f, 0.0f, 0.0f);

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}

void RigidBody::Transform()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	this->collider->Transform(GetWorldMatrix());

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}

void RigidBody::SetAcceleration(const Vector3D& acceleration)
{
	this->acceleration = acceleration;
}

const Vector3D RigidBody::GetAcceleration()
{
	return this->acceleration;
}

void RigidBody::SetVelocity(const Vector3D& velocity)
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	if (std::isnan(velocity.x) || std::isnan(velocity.y) || std::isnan(velocity.z))
	{
		this->velocity = Vector3D(0.0f, 0.0f, 0.0f);
	}
	else
	{
		this->velocity = velocity;
	}

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}

const Vector3D RigidBody::GetVelocity()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	Vector3D v = velocity;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return v;
}

const float RigidBody::GetDampingRatio()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	float f = 0.0f;
	if (elasticity != 0.0f)
	{
		f = 2 * sqrt(mass * elasticity);
	}

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return f;
}

void RigidBody::SetElasticity(const float& elasticity)
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	this->elasticity = elasticity;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}

const float RigidBody::GetElasticity()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	float f = this->elasticity;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return f;
}

const float RigidBody::GetInverseMass()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	float f = this->inverseMass;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return f;
}

void RigidBody::SetMass(const float& mass)
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	this->mass = mass;

	if (this->mass == 0.0f)
	{
		inverseMass = 1.0f;
	}
	else
	{
		inverseMass = 1.0f / this->mass;
	}

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}

const float RigidBody::GetMass()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	float f = this->mass;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return f;
}

void RigidBody::ApplyForce(const Vector3D& force)
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	this->acceleration += force * this->inverseMass;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}

const std::string RigidBody::GetType()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	std::string s = RIGIDBODY_STR;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return s;
}

const unsigned long long RigidBody::GetTypeID()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	unsigned long long i = ENGINE_OBJ_ID_PHY_RIGIDBODY;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return i;
}

const bool RigidBody::IsFixed()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	bool b = GetMass() == 0.0f;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return b;
}

const std::string RigidBody::GetName()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	std::string s = this->name;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return s;
}

void RigidBody::SetName(const std::string& name)
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	this->name = name;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}

void RigidBody::Dispose()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	DisposePointer(transform);
	DisposePointer(collider);

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}

void RigidBody::Update(const double& deltaTime)
{
	Transform();
}