#include "SoftbodyFactory.h"
#include "Softbody.h"
#include <StringHelpers.h>
#include "ColliderFactory.h"

SoftbodyFactory::SoftbodyFactory()
{
	colliderFactory = new ColliderFactory();
}
SoftbodyFactory::~SoftbodyFactory()
{
	Dispose();
}
/*Creates the mass-spring softbody.*/
ISoftbody* SoftbodyFactory::CreateSoftbody(ParameterContainer& parameters)
{
	/*
	(1.) Instead of having Defs everywhere, I use a collection of generic parameters.
	(2.) These are read from the XML file and passed to this factory, which converts the string data into
	their actual types.
	(3.) The values are then used to create the softbody, its nodes and springs and their relationships.
	*/

	Parameter p;
	bool rc = false;

	//Softbody transform properties
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

	rc = parameters.FindParameterByName("NodeMass", p);
	if (!rc) return nullptr;
	float mass = strtof(p.value.c_str(), NULL);

	rc = parameters.FindParameterByName("NodeElasticity", p);
	if (!rc) return nullptr;
	float elasticity = strtof(p.value.c_str(), NULL);

	//Softbody properties
	bool allowCollisionDetection = true;
	rc = parameters.FindParameterByName("AllowCollisionDetection", p);
	if (rc) allowCollisionDetection = strtoul(p.value.c_str(), NULL, 0) != 0;

	rc = parameters.FindParameterByName("SpringConstant", p);
	if (!rc) return nullptr;
	float springConstant = strtof(p.value.c_str(), NULL);

	rc = parameters.FindParameterByName("CornerA", p);
	if (!rc) return nullptr;
	Vector3D cornerA = StringHelpers::StrToVec3(p.value);

	rc = parameters.FindParameterByName("CornerB", p);
	if (!rc) return nullptr;
	Vector3D cornerB = StringHelpers::StrToVec3(p.value);
		
	rc = parameters.FindParameterByName("DownDirection", p);
	if (!rc) return nullptr;
	Vector3D downDirection = StringHelpers::StrToVec3(p.value);

	rc = parameters.FindParameterByName("NodeCount", p);
	if (!rc) return nullptr;
	size_t nodeCount = strtoull(p.value.c_str(), NULL, 0);

	rc = parameters.FindParameterByName("NodesAcrossCount", p);
	if (!rc) return nullptr;
	size_t nodesAcrossCount = strtoull(p.value.c_str(), NULL, 0);

	rc = parameters.FindParameterByName("NodesDownCount", p);
	if (!rc) return nullptr;
	size_t nodesDownCount = strtoull(p.value.c_str(), NULL, 0);

	void* parent = nullptr;
	rc = parameters.FindParameterByName("parent", p);
	if (rc) parent = p.valuePtr;

	Vector3D separationAcross = cornerB - cornerA;
	separationAcross /= nodesAcrossCount;

	Vector3D separationDown = glm::normalize(downDirection);
	separationDown *= glm::length(separationAcross);

	//Rigidbody and collider properties
	Softbody* softbody = new Softbody();
	softbody->SetParent(parent);
	softbody->SetPosition(position);
	softbody->SetRotation(rotation);
	softbody->SetScale(scale);
	softbody->SetOrientation(orientation);
	softbody->SetUp(Vector3D(up.x, up.y, up.z));
	softbody->SetColumnCount(nodesAcrossCount);
	softbody->SetRowCount(nodesDownCount);

	//SEND NODES!
	char rbname[255];
	unsigned int nodeID = 0;
	for (size_t idxDown = 0; idxDown < nodesDownCount; idxDown++)
	{
		for (size_t idxAcross = 0; idxAcross < nodesAcrossCount; idxAcross++)
		{
			sprintf_s(rbname, 255, "Rigidbody_Node_%d", nodeID);

			ICollider* collider = colliderFactory->Create(parameters);
			if (!collider) continue;

			Softbody::Node* node = new Softbody::Node();
			if (!node)
			{
				delete collider;
				continue;
			}
			node->SetID(nodeID);
			nodeID++;

			RigidBody* rb = new RigidBody();
			if (!rb)
			{
				delete collider;
				delete node;
				continue;
			}
			rb->AddParent(softbody);
			rb->SetName(rbname);
			rb->SetCollider(collider);
			rb->SetPosition(cornerA + separationAcross * (float)idxAcross + separationDown * (float)idxDown);
			rb->SetUp(Vector3D(up.x, up.y, up.z));
			rb->AllowUpTransform(allowUpTransform);
			rb->SetOrientation(orientation);
			rb->SetRotation(rotation);
			rb->SetScale(scale);
			rb->SetVelocity(Vector3D());
			rb->SetAcceleration(Vector3D());
			/*The first row is always static, so if idxDown is 0, make the rigidbody static by having mass=zero!*/
			rb->SetMass(idxDown != 0 ? mass : 0.0f);
			rb->SetElasticity(elasticity);
			rb->ToggleCollisionDetection(allowCollisionDetection);

			node->AddComponent(rb);
			if (!softbody->AddNode(node))
			{
				delete collider;
				delete node;
				delete rb;
				continue;
			}			
		}
	}
		
	//Creates the spring connections
	for (size_t idxDown = 0; idxDown < nodesDownCount - 1; idxDown++)
	{
		for (size_t idxAcross = 0; idxAcross < nodesAcrossCount - 1; idxAcross++)
		{
			size_t idxNode = (idxDown * nodesAcrossCount) + idxAcross;

			softbody->AddConnection(idxNode, idxNode + 1);
			softbody->AddConnection(idxNode, idxNode + nodesAcrossCount);
		}
	}

	//Set bottom row springs
	size_t bottomRowStart = nodesAcrossCount * (nodesDownCount - 1);
	for (size_t idxAcross = 0; idxAcross < nodesAcrossCount - 1; idxAcross++)
	{
		softbody->AddConnection(bottomRowStart + idxAcross, bottomRowStart + idxAcross + 1);
	}
	//Set right row springs
	for (size_t idxDown = 0; idxDown < nodesDownCount - 1; idxDown++)
	{
		size_t idA = nodesAcrossCount * (idxDown + 1) - 1;
		size_t idB = idA + nodesAcrossCount;
		softbody->AddConnection(idA, idB);
	}

	//Creates the springs and the relationship between the nodes.
	std::vector<std::pair<unsigned int, unsigned int>> connections;
	if (softbody->GetConnections(connections))
	{
		for (std::pair<unsigned int, unsigned int> element : connections)
		{
			Softbody::Node* nodeA = softbody->FindNodeByID(element.first);			
			Softbody::Node* nodeB = softbody->FindNodeByID(element.second);

			Softbody::Spring* spr = new Softbody::Spring(nodeA, nodeB);
			spr->SetSpringConstant(springConstant);

			nodeA->AddSpring(spr);
			nodeB->AddSpring(spr);

			softbody->AddSpring(spr);
		}
	}

	return softbody;
}

ISoftbody* SoftbodyFactory::CreateCloth(ParameterContainer& parameters)
{
	return nullptr;
}

ISoftbody* SoftbodyFactory::Create(ParameterContainer& parameters)
{
	Parameter p;

	if (parameters.FindParameterByName("Type", p))
	{
		if (p.value == "Softbody")
		{
			return CreateSoftbody(parameters);
		}
		else if (p.value == "ClothComponent")
		{
			return CreateCloth(parameters);
		}
		else
		{
			return nullptr;
		}
	}

	return nullptr;
}

const bool SoftbodyFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return false;
}

void SoftbodyFactory::Dispose()
{
	DisposePointer(colliderFactory);
}
