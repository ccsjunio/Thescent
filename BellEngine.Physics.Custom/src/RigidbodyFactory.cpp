#include "RigidbodyFactory.h"
#include "RigidBody.h"
#include <StringHelpers.h>

RigidbodyFactory::RigidbodyFactory()
{
}

RigidbodyFactory::~RigidbodyFactory()
{
}

IRigidbody* RigidbodyFactory::Create(ParameterContainer& parameters)
{
	Parameter p;
	bool rc = false;

	rc = parameters.FindParameterByName("Position", p);
	if (!rc) return nullptr;
	Vector3D position = StringHelpers::StrToVec3(p.value);

	rc = parameters.FindParameterByName("Up", p);
	if (!rc) return nullptr;
	Vector4D up = StringHelpers::StrToVec4(p.value);
	bool allowUpTransform = up.w != 0.0f;

	rc = parameters.FindParameterByName("Orientation", p);
	if (!rc) return nullptr;
	Vector3D orientation = StringHelpers::StrToVec3(p.value);

	rc = parameters.FindParameterByName("Rotation", p);
	if (!rc) return nullptr;
	Vector3D rotation = StringHelpers::StrToVec3(p.value);

	rc = parameters.FindParameterByName("Scale", p);
	if (!rc) return nullptr;
	Vector3D scale = StringHelpers::StrToVec3(p.value);

	rc = parameters.FindParameterByName("Velocity", p);
	if (!rc) return nullptr;
	Vector3D velocity = StringHelpers::StrToVec3(p.value);

	rc = parameters.FindParameterByName("Acceleration", p);
	if (!rc) return nullptr;
	Vector3D acceleration = StringHelpers::StrToVec3(p.value);

	rc = parameters.FindParameterByName("Mass", p);
	if (!rc) return nullptr;
	float mass = atof(p.value.c_str());

	rc = parameters.FindParameterByName("Elasticity", p);
	if (!rc) return nullptr;
	float elasticity = atof(p.value.c_str());

	rc = parameters.FindParameterByName("AllowCollisionDetection", p);
	if (!rc) return nullptr;
	bool allowCollisionDetection = atoi(p.value.c_str()) != 0;

	void* parent = nullptr;
	rc = parameters.FindParameterByName("parent", p);
	if (rc) parent = p.valuePtr;

	RigidBody* rb = new RigidBody();
	rb->AddParent(parent);
	rb->SetName("RigidBody");
	rb->SetPosition(position);
	rb->SetUp(Vector3D(up.x, up.y, up.z));
	rb->AllowUpTransform(allowUpTransform);
	rb->SetOrientation(orientation);
	rb->SetRotation(rotation);
	rb->SetScale(scale);
	rb->SetVelocity(velocity);
	rb->SetAcceleration(acceleration);
	rb->SetMass(mass);
	rb->SetElasticity(elasticity);
	rb->ToggleCollisionDetection(allowCollisionDetection);

	return rb;
}

const bool RigidbodyFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return true;
}
