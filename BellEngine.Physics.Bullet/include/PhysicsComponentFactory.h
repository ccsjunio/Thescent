#pragma once
#include <IFactory.h>
#include <IPhysicsComponent.h>
#include <IDisposable.h>
#include <IRigidbody.h>
#include <ISoftbody.h>
#include <ICollider.h>

class PhysicsComponentFactory : public IFactory<IPhysicsComponent>, public IDisposable
{
private:
	IFactory<IRigidbody>* rigidBodyFactory;
	IFactory<ISoftbody>* softBodyFactory;
	IFactory<ICollider>* colliderFactory;

public:
	PhysicsComponentFactory();
	~PhysicsComponentFactory();

	IPhysicsComponent* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
	void Dispose();
};


