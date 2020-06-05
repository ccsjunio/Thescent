#include "PhysicsComponentFactory.h"
#include "RigidbodyFactory.h"
#include "SoftbodyFactory.h"
#include "ColliderFactory.h"

PhysicsComponentFactory::PhysicsComponentFactory()
{
	rigidBodyFactory = new RigidbodyFactory();
	softBodyFactory = new SoftbodyFactory();
	colliderFactory = new ColliderFactory();
}

PhysicsComponentFactory::~PhysicsComponentFactory()
{
	Dispose();
}

IPhysicsComponent* PhysicsComponentFactory::Create(ParameterContainer& parameters)
{
	Parameter p;

	if (parameters.FindParameterByName("Type", p))
	{
		if (p.value == "Rigidbody")
		{
			IRigidbody* rb = rigidBodyFactory->Create(parameters);
			if (rb)
			{
				Parameter p;
				if (parameters.FindParameterByName("UseWorldMatrix", p))
				{
					Matrix4D wm = rb->GetWorldMatrix();
					parameters.Add(Parameter("Matrix4D", "rigidbody_world_matrix", &wm, false));
				}

				ICollider* collider = colliderFactory->Create(parameters);
				if (collider)
				{
					rb->SetCollider(collider);
					collider->SetParent(rb);
					rb->Transform();
				}
			
				return rb;
			}
			else
			{
				return nullptr;
			}
		}
		else if (p.value == "Softbody")
		{
			return softBodyFactory->Create(parameters);
		}
		else
		{
			return nullptr;
		}
	}

	return nullptr;
}

const bool PhysicsComponentFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return false;
}

void PhysicsComponentFactory::Dispose()
{
	DisposePointer(colliderFactory);
	DisposePointer(rigidBodyFactory);
	DisposePointer(softBodyFactory);
}
