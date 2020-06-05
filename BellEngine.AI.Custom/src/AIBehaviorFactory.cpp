#include "AIBehaviorFactory.h"
#include <StringHelpers.h>
#include <Entity.h>
#include "../AIBehavior/include/IPathFinding.h"
#include "../AIBehavior/include/EvadeByMatch.h"
#include "../AIBehavior/include/Pursue.h"
#include "../AIBehavior/include/Seek.h"
#include "../AIBehavior/include/Flee.h"
#include "../AIBehavior/include/Evade.h"
#include "../AIBehavior/include/Wander.h"
#include "../AIBehavior/include/LookAtTarget.h"
#include "../AIBehavior/include/LookAtMovingDirection.h"
#include "../AIBehavior/include/ShootAt.h"
#include "../AIBehavior/include/Flocking.h"
#include "../AIBehavior/include/PathFollowing.h"
#include "../AIBehavior/include/Formation.h"
#include "../AIBehavior/include/AxisRotation.h"
#include "../AIBehavior/include/PathFindingAStar.h"
#include "../AIBehavior/include/PathFindingDjikstra.h"
#include "../AIBehavior/include/PathFindingDepthFirst.h"
#include "../AIBehavior/include/PathFindingBreadthFirst.h"

IAIBehavior* AIBehaviorFactory::CreateSeek(ParameterContainer& parameters)
{
	Parameter temp;
	bool rc = false;

	Seek* ai = new Seek();
	rc = parameters.FindParameterByName("Target", temp);
	if (rc) ai->target = (Entity*)temp.valuePtr;
	rc = parameters.FindParameterByName("Parent", temp);
	if (rc) ai->AddParent((Entity*)temp.valuePtr);
	rc = parameters.FindParameterByName("SlowingDistance", temp);
	if (rc) ai->slowingDistance = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("MinDistance", temp);
	if (rc) ai->minDistance = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("MaxDistance", temp);
	if (rc) ai->maxDistance = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("TopSpeed", temp);
	if (rc) ai->topSpeed = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("IsActive", temp);
	if (rc) ai->Activate(atoi(temp.value.c_str()) != 0);

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateFlee(ParameterContainer& parameters)
{
	Parameter temp;
	bool rc = false;

	Flee* ai = new Flee();
	rc = parameters.FindParameterByName("Target", temp);
	if (rc) ai->target = (Entity*)temp.valuePtr;
	rc = parameters.FindParameterByName("Parent", temp);
	if (rc) ai->AddParent((Entity*)temp.valuePtr);
	rc = parameters.FindParameterByName("SlowingDistance", temp);
	if (rc) ai->slowingDistance = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("MinDistance", temp);
	if (rc) ai->minDistance = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("MaxDistance", temp);
	if (rc) ai->maxDistance = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("TopSpeed", temp);
	if (rc) ai->topSpeed = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("IsActive", temp);
	if (rc) ai->Activate(atoi(temp.value.c_str()) != 0);

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreatePursue(ParameterContainer& parameters)
{
	Parameter p;

	Entity* target = nullptr;
	if (!parameters.FindParameterByName("Target", p)) return nullptr;
	target = (Entity*)p.valuePtr;

	Entity* parent = nullptr;
	if (!parameters.FindParameterByName("Parent", p)) return nullptr;
	parent = (Entity*)p.valuePtr;

	float slowingDistance = 0.0f;
	if(parameters.FindParameterByName("SlowingDistance", p)) slowingDistance = strtof(p.value.c_str(), NULL);

	float minDistance = 0.0f;
	if(parameters.FindParameterByName("MinDistance", p)) minDistance = strtof(p.value.c_str(), NULL);

	float maxDistance = 0.0f;
	if (parameters.FindParameterByName("MaxDistance", p)) maxDistance = strtof(p.value.c_str(), NULL);

	float topSpeed = 0.0f;
	if (!parameters.FindParameterByName("TopSpeed", p)) return nullptr;
	topSpeed = strtof(p.value.c_str(), NULL);

	bool isActive = true;
	if (parameters.FindParameterByName("IsActive", p)) isActive = strtoul(p.value.c_str(), NULL, 0);

	Vector3D axisLock = Vector3D(0.0f, 0.0f, 0.0f);
	if (parameters.FindParameterByName("AxisLock", p)) axisLock = StringHelpers::StrToVec3(p.value);

	Pursue* ai = new Pursue();
	ai->SetTarget(target);
	ai->AddParent(parent);
	ai->SetSlowingDistance(slowingDistance);
	ai->SetMinDistance(minDistance);
	ai->SetMaxDistance(maxDistance);
	ai->SetTopSpeed(topSpeed);
	ai->SetAxisLock(axisLock.x != 0.0f, axisLock.y != 0.0f, axisLock.z != 0.0f);
	ai->Activate(isActive);

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateEvade(ParameterContainer& parameters)
{
	Parameter temp;
	bool rc = false;

	Evade* ai = new Evade();
	rc = parameters.FindParameterByName("Target", temp);
	if (rc) ai->target = (Entity*)temp.valuePtr;
	rc = parameters.FindParameterByName("Parent", temp);
	if (rc) ai->AddParent((Entity*)temp.valuePtr);
	rc = parameters.FindParameterByName("SlowingDistance", temp);
	if (rc) ai->slowingDistance = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("MinDistance", temp);
	if (rc) ai->minDistance = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("MaxDistance", temp);
	if (rc) ai->maxDistance = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("TopSpeed", temp);
	if (rc) ai->topSpeed = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("IsActive", temp);
	if (rc) ai->Activate(atoi(temp.value.c_str()) != 0);

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateEvadeByMatch(ParameterContainer& parameters)
{
	Parameter p;
		
	std::string name;
	if (parameters.FindParameterByName("Name", p)) name = p.value;

	std::string targetNameMatch;
	if (!parameters.FindParameterByName("TargetNameMatch", p)) return nullptr;
	targetNameMatch = p.value;

	Entity* parent = nullptr;
	if (!parameters.FindParameterByName("Parent", p)) return nullptr;
	parent = (Entity*)p.valuePtr;

	Scene* scene = parent->GetParent<Scene*>();
	if (!scene) return nullptr;

	float slowingDistance = 0.0f;
	if (parameters.FindParameterByName("SlowingDistance", p)) slowingDistance = strtof(p.value.c_str(), NULL);

	float minDistance = 0.0f;
	if (parameters.FindParameterByName("MinDistance", p)) minDistance = strtof(p.value.c_str(), NULL);

	float maxDistance = 0.0f;
	if (parameters.FindParameterByName("MaxDistance", p)) maxDistance = strtof(p.value.c_str(), NULL);

	float topSpeed = 0.0f;
	if (!parameters.FindParameterByName("TopSpeed", p)) return nullptr;
	topSpeed = strtof(p.value.c_str(), NULL);

	bool isActive = true;
	if (parameters.FindParameterByName("IsActive", p)) isActive = strtoul(p.value.c_str(), NULL, 0);

	EvadeByMatch* ai = new EvadeByMatch();
	ai->SetName(name);
	ai->SetNameMatch(targetNameMatch);
	ai->SetScene(scene);
	ai->AddParent(parent);
	ai->SetSlowingDistance(slowingDistance);
	ai->SetMinDistance(minDistance);
	ai->SetMaxDistance(maxDistance);
	ai->SetTopSpeed(topSpeed);
	ai->Activate(isActive);

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateWander(ParameterContainer& parameters)
{
	Parameter temp;
	bool rc = false;

	Wander* ai = new Wander();
	rc = parameters.FindParameterByName("TargetPosition", temp);
	if (rc) ai->targetPosition = StringHelpers::StrToVec3(temp.value);
	rc = parameters.FindParameterByName("Parent", temp);
	if (rc) ai->AddParent((Entity*)temp.valuePtr);
	rc = parameters.FindParameterByName("DistanceToCircle", temp);
	if (rc) ai->distanceToCircle = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("CircleRadius", temp);
	if (rc) ai->circleRadius = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("TopSpeed", temp);
	if (rc) ai->topSpeed = strtof(temp.value.c_str(), NULL);
	rc = parameters.FindParameterByName("IsActive", temp);
	if (rc) ai->Activate(atoi(temp.value.c_str()) != 0);

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateLookAtTarget(ParameterContainer& parameters)
{
	Parameter p;
	bool rc = false;

	Vector3D axisLock = Vector3D(0.0f, 0.0f, 0.0f);
	if (parameters.FindParameterByName("AxisLock", p)) axisLock = StringHelpers::StrToVec3(p.value);

	LookAtTarget* ai = new LookAtTarget();
	rc = parameters.FindParameterByName("TargetPosition", p);
	if (rc) ai->SetTargetPosition(StringHelpers::StrToVec3(p.value));
	rc = parameters.FindParameterByName("Parent", p);
	if (rc) ai->AddParent((Entity*)p.valuePtr);
	rc = parameters.FindParameterByName("Target", p);
	if (rc) ai->SetTarget((Entity*)p.valuePtr);
	rc = parameters.FindParameterByName("IsActive", p);
	if (rc) ai->Activate(strtoul(p.value.c_str(), NULL, 0) != 0);
	rc = parameters.FindParameterByName("Radius", p);
	if (rc) ai->SetRadius(strtof(p.value.c_str(), NULL));
	rc = parameters.FindParameterByName("RotationalGain", p);
	if (rc) ai->SetRotationalGain(strtof(p.value.c_str(), NULL));
	ai->SetAxisLock(axisLock.x != 0.0f, axisLock.y != 0.0f, axisLock.z != 0.0f);

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateLookAtMovingDirection(ParameterContainer& parameters)
{
	Parameter temp;
	bool rc = false;

	LookAtMovingDirection* ai = new LookAtMovingDirection();
	rc = parameters.FindParameterByName("Parent", temp);
	if (rc) ai->AddParent((Entity*)temp.valuePtr);	
	rc = parameters.FindParameterByName("IsActive", temp);
	if (rc) ai->Activate(atoi(temp.value.c_str()) != 0);

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreatePathFollowing(ParameterContainer& parameters)
{
	Parameter temp;
	bool rc = false;

	PathFollowing* ai = nullptr;

	unsigned int nodeCount = 0;
	rc = parameters.FindParameterByName("NodeCount", temp);
	if (rc)
	{
		nodeCount = atoi(temp.value.c_str());

		if (nodeCount > 0)
		{
			ai = new PathFollowing();

			rc = parameters.FindParameterByName("Name", temp);
			rc ? ai->SetName(temp.value) : ai->SetName(StringHelpers::GenerateRandomString(4));

			rc = parameters.FindParameterByName("IsActive", temp);
			if (rc) ai->Activate(atoi(temp.value.c_str()) != 0);

			rc = parameters.FindParameterByName("Loop", temp);
			if (rc) ai->Loop(strtoul(temp.value.c_str(), NULL, 0) != 0);

			rc = parameters.FindParameterByName("Parent", temp);
			if (rc) ai->AddParent((Entity*)temp.valuePtr);

			rc = parameters.FindParameterByName("TopSpeed", temp);
			if (rc) ai->SetTopSpeed(strtof(temp.value.c_str(), NULL));

			rc = parameters.FindParameterByName("NodeRadius", temp);
			if (rc) ai->SetNodeRadius(strtof(temp.value.c_str(), NULL));

			for (unsigned int i = 0; i < nodeCount; i++)
			{
				rc = parameters.FindParameterByName("Node" + std::to_string(i), temp);
				if (rc)
				{
					rc = ai->AddPathNode(StringHelpers::StrToVec3(temp.value));
					if (!rc)
					{
						//Something went wrong
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateFlocking(ParameterContainer& parameters)
{
	Parameter temp;
	bool rc = false;

	Flocking* ai = nullptr;

	unsigned int boidCount = 0;
	rc = parameters.FindParameterByName("BoidCount", temp);
	if (rc)
	{
		boidCount = atoi(temp.value.c_str());

		if (boidCount > 0)
		{
			ai = new Flocking();

			rc = parameters.FindParameterByName("Name", temp);
			rc ? ai->SetName(temp.value) : ai->SetName(StringHelpers::GenerateRandomString(4));

			rc = parameters.FindParameterByName("IsActive", temp);
			if (rc) ai->Activate(atoi(temp.value.c_str()) != 0);

			rc = parameters.FindParameterByName("CohesionWeight", temp);
			if (rc) ai->SetCohesionWeight(strtof(temp.value.c_str(), NULL));

			rc = parameters.FindParameterByName("AlignmentWeight", temp);
			if (rc) ai->SetAlignmentWeight(strtof(temp.value.c_str(), NULL));

			rc = parameters.FindParameterByName("SeparationWeight", temp);
			if (rc) ai->SetSeparationWeight(strtof(temp.value.c_str(), NULL));

			rc = parameters.FindParameterByName("TopSpeed", temp);
			if (rc) ai->SetTopSpeed(strtof(temp.value.c_str(), NULL));

			for (unsigned int i = 0; i < boidCount; i++)
			{
				rc = parameters.FindParameterByName("Boid" + std::to_string(i), temp);
				if (rc)
				{
					rc = ai->AddBoid((Entity*)temp.valuePtr);
					if (!rc)
					{
						//Something went wrong
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateFormation(ParameterContainer& parameters)
{
	Parameter temp;
	bool rc = false;

	Formation* ai = nullptr;

	//NODE COUNT
	unsigned int nodeCount = 0;
	rc = parameters.FindParameterByName("NodeCount", temp);
	if (!rc) return nullptr;
	nodeCount = atoi(temp.value.c_str());
	if (nodeCount == 0) return nullptr;

	//BOID COUNT
	unsigned int boidCount = 0;
	rc = parameters.FindParameterByName("BoidCount", temp);
	if (!rc) return nullptr;
	boidCount = atoi(temp.value.c_str());
	if (boidCount == 0) return nullptr;

	//Valid formation behavior
	ai = new Formation();

	rc = parameters.FindParameterByName("Name", temp);
	rc ? ai->SetName(temp.value) : ai->SetName(StringHelpers::GenerateRandomString(4));

	rc = parameters.FindParameterByName("IsActive", temp);
	if (rc) ai->Activate(atoi(temp.value.c_str()) != 0);

	rc = parameters.FindParameterByName("CoordinatorTarget", temp);
	if (rc) ai->SetCoordinatorTarget(StringHelpers::StrToVec3(temp.value));

	rc = parameters.FindParameterByName("CoordinatorPosition", temp);
	if (rc) ai->SetCoordinatorPosition(StringHelpers::StrToVec3(temp.value));

	//NODES
	for (unsigned int i = 0; i < nodeCount; i++)
	{
		rc = parameters.FindParameterByName("Node" + std::to_string(i), temp);
		if (rc)
		{
			rc = ai->AddFormationNode(StringHelpers::StrToVec3(temp.value));
			if (!rc)
			{
				//Something went wrong
			}
		}
	}

	//BOIDS
	for (unsigned int i = 0; i < boidCount; i++)
	{
		rc = parameters.FindParameterByName("Boid" + std::to_string(i), temp);
		if (rc)
		{
			rc = ai->AddBoid((Entity*)temp.valuePtr);
			if (!rc)
			{
				//Something went wrong
			}
		}
	}

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateBreadthFirstSearch(ParameterContainer& parameters)
{
	Parameter p;
	bool rc = false;

	std::string name;
	rc = parameters.FindParameterByName("Name", p);
	rc ? name = p.value : name = StringHelpers::GenerateRandomString(4);

	rc = parameters.FindParameterByName("Parent", p);
	if (!rc) return nullptr;
	Entity* parent = (Entity*)p.valuePtr;

	bool isActive = false;
	rc = parameters.FindParameterByName("IsActive", p);
	if (rc) isActive = strtoul(p.value.c_str(), NULL, 0) != 0;

	float topSpeed = 0.0f;
	rc = parameters.FindParameterByName("TopSpeed", p);
	if (rc) topSpeed = strtof(p.value.c_str(), NULL);

	float nodeRadius = 0.0f;
	rc = parameters.FindParameterByName("NodeRadius", p);
	if (rc) nodeRadius = strtof(p.value.c_str(), NULL);

	rc = parameters.FindParameterByName("NodeCount", p);
	if (!rc) return nullptr;

	unsigned long nodeCount = strtoul(p.value.c_str(), NULL, 0);
	if (nodeCount == 0) return nullptr;

	rc = parameters.FindParameterByName("EdgeCount", p);
	if (!rc) return nullptr;

	unsigned long edgeCount = strtoul(p.value.c_str(), NULL, 0);
	if (edgeCount == 0) return nullptr;

	rc = parameters.FindParameterByName("RootNodeID", p);
	if (!rc) return nullptr;

	unsigned long rootNodeID = strtoul(p.value.c_str(), NULL, 0);
	if (edgeCount == 0) return nullptr;

	unsigned int nodeID = 0ul;
	Vector3D position = Vector3D();
	std::vector<std::pair<unsigned int/*id*/, std::pair<bool /*has goal*/, Vector3D /*position*/>>> nodes;
	for (unsigned long i = 0; i < nodeCount; i++)
	{
		rc = parameters.FindParameterByName("Node" + std::to_string(i), p);
		if (!rc) continue;		
		nodeID = strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("NodePosition" + std::to_string(i), p);
		if (!rc) continue;
		position = StringHelpers::StrToVec3(p.value);

		bool hasGoal = false;
		rc = parameters.FindParameterByName("NodeGoal" + std::to_string(i), p);
		if(rc) hasGoal = strtoul(p.value.c_str(), NULL, 0) != 0;

		nodes.push_back(std::pair<unsigned int, std::pair<bool, Vector3D>>(nodeID, std::pair<bool, Vector3D>(hasGoal, position)));
	}

	if (nodes.empty()) return nullptr;
	
	unsigned int parentID = 0ul, childID = 0ul, weight = 0ul;
	std::vector<std::pair<unsigned int/*weight*/, std::pair<unsigned int/*parent id*/, unsigned int/*child id*/>>> edges;
	for (unsigned long i = 0; i < edgeCount; i++)
	{
		rc = parameters.FindParameterByName("EdgeParentID" + std::to_string(i), p);
		if (!rc) continue;
		parentID = strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("EdgeChildID" + std::to_string(i), p);
		if (!rc) continue;
		childID = strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("EdgeWeight" + std::to_string(i), p);
		if (!rc) continue;
		weight = strtoul(p.value.c_str(), NULL, 0);

		edges.push_back(std::pair<unsigned int, std::pair<unsigned int, unsigned int>>(weight, std::pair<unsigned int, unsigned int>(parentID, childID)));
	}

	if (edges.empty()) return nullptr;

	Graph::Node* rootNode = nullptr;
	Graph* graph = new Graph();
	for (auto pair : nodes)
	{
		Graph::Node* node = new Graph::Node();
		node->SetID(pair.first);
		node->SetIfHasGoal(pair.second.first);
		node->SetPosition(pair.second.second);
		graph->AddNode(node);

		if (node->GetID() == rootNodeID)
		{
			rootNode = node;
		}
	}

	for (auto element : edges)
	{
		Graph::Node* parentNode = graph->FindNodeByID(element.second.first);
		Graph::Node* childNode = graph->FindNodeByID(element.second.second);
		graph->AddEdge(parentNode, childNode, (float)element.first);
	}

	PathFindingBreadthFirst* ai = new PathFindingBreadthFirst();
	ai->SetName(name);
	ai->SetGraph(graph);
	ai->Activate(isActive);
	ai->AddParent(parent);
	ai->SetRootNode(rootNode);
	if (nodeRadius != 0.0f) ai->SetNodeRadius(nodeRadius);
	if (topSpeed != 0.0f) ai->SetTopSpeed(topSpeed);

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateDepthFirstSearch(ParameterContainer& parameters)
{
	Parameter p;
	bool rc = false;

	std::string name;
	rc = parameters.FindParameterByName("Name", p);
	rc ? name = p.value : name = StringHelpers::GenerateRandomString(4);

	rc = parameters.FindParameterByName("Parent", p);
	if (!rc) return nullptr;
	Entity* parent = (Entity*)p.valuePtr;

	bool isActive = false;
	rc = parameters.FindParameterByName("IsActive", p);
	if (rc) isActive = strtoul(p.value.c_str(), NULL, 0) != 0;

	float topSpeed = 0.0f;
	rc = parameters.FindParameterByName("TopSpeed", p);
	if (rc) topSpeed = strtof(p.value.c_str(), NULL);

	float nodeRadius = 0.0f;
	rc = parameters.FindParameterByName("NodeRadius", p);
	if (rc) nodeRadius = strtof(p.value.c_str(), NULL);

	rc = parameters.FindParameterByName("NodeCount", p);
	if (!rc) return nullptr;

	unsigned long nodeCount = strtoul(p.value.c_str(), NULL, 0);
	if (nodeCount == 0) return nullptr;

	rc = parameters.FindParameterByName("EdgeCount", p);
	if (!rc) return nullptr;

	unsigned long edgeCount = strtoul(p.value.c_str(), NULL, 0);
	if (edgeCount == 0) return nullptr;

	rc = parameters.FindParameterByName("RootNodeID", p);
	if (!rc) return nullptr;

	unsigned long rootNodeID = strtoul(p.value.c_str(), NULL, 0);
	if (edgeCount == 0) return nullptr;

	unsigned int nodeID = 0ul;
	Vector3D position = Vector3D();
	std::vector<std::pair<unsigned int/*id*/, std::pair<bool /*has goal*/, Vector3D /*position*/>>> nodes;
	for (unsigned long i = 0; i < nodeCount; i++)
	{
		rc = parameters.FindParameterByName("Node" + std::to_string(i), p);
		if (!rc) continue;
		nodeID = strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("NodePosition" + std::to_string(i), p);
		if (!rc) continue;
		position = StringHelpers::StrToVec3(p.value);

		bool hasGoal = false;
		rc = parameters.FindParameterByName("NodeGoal" + std::to_string(i), p);
		if (rc) hasGoal = strtoul(p.value.c_str(), NULL, 0) != 0;

		nodes.push_back(std::pair<unsigned int, std::pair<bool, Vector3D>>(nodeID, std::pair<bool, Vector3D>(hasGoal, position)));
	}

	if (nodes.empty()) return nullptr;

	unsigned int parentID = 0ul, childID = 0ul, weight = 0ul;
	std::vector<std::pair<unsigned int/*weight*/, std::pair<unsigned int/*parent id*/, unsigned int/*child id*/>>> edges;
	for (unsigned long i = 0; i < edgeCount; i++)
	{
		rc = parameters.FindParameterByName("EdgeParentID" + std::to_string(i), p);
		if (!rc) continue;
		parentID = strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("EdgeChildID" + std::to_string(i), p);
		if (!rc) continue;
		childID = strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("EdgeWeight" + std::to_string(i), p);
		if (!rc) continue;
		weight = strtoul(p.value.c_str(), NULL, 0);

		edges.push_back(std::pair<unsigned int, std::pair<unsigned int, unsigned int>>(weight, std::pair<unsigned int, unsigned int>(parentID, childID)));
	}

	if (edges.empty()) return nullptr;

	Graph::Node* rootNode = nullptr;
	Graph* graph = new Graph();
	for (auto pair : nodes)
	{
		Graph::Node* node = new Graph::Node();
		node->SetID(pair.first);
		node->SetIfHasGoal(pair.second.first);
		node->SetPosition(pair.second.second);
		graph->AddNode(node);

		if (node->GetID() == rootNodeID)
		{
			rootNode = node;
		}
	}

	for (auto element : edges)
	{
		Graph::Node* parentNode = graph->FindNodeByID(element.second.first);
		Graph::Node* childNode = graph->FindNodeByID(element.second.second);
		graph->AddEdge(parentNode, childNode, (float)element.first);
	}

	PathFindingDepthFirst* ai = new PathFindingDepthFirst();
	ai->SetName(name);
	ai->SetGraph(graph);
	ai->Activate(isActive);
	ai->AddParent(parent);
	ai->SetRootNode(rootNode);
	if (nodeRadius != 0.0f) ai->SetNodeRadius(nodeRadius);
	if (topSpeed != 0.0f) ai->SetTopSpeed(topSpeed);

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateDjikstraSearch(ParameterContainer& parameters)
{
	Parameter p;
	bool rc = false;

	std::string name;
	rc = parameters.FindParameterByName("Name", p);
	rc ? name = p.value : name = StringHelpers::GenerateRandomString(4);

	rc = parameters.FindParameterByName("Parent", p);
	if (!rc) return nullptr;
	Entity* parent = (Entity*)p.valuePtr;

	bool isActive = false;
	rc = parameters.FindParameterByName("IsActive", p);
	if (rc) isActive = strtoul(p.value.c_str(), NULL, 0) != 0;

	float topSpeed = 0.0f;
	rc = parameters.FindParameterByName("TopSpeed", p);
	if (rc) topSpeed = strtof(p.value.c_str(), NULL);

	float nodeRadius = 0.0f;
	rc = parameters.FindParameterByName("NodeRadius", p);
	if (rc) nodeRadius = strtof(p.value.c_str(), NULL);

	rc = parameters.FindParameterByName("NodeCount", p);
	if (!rc) return nullptr;

	unsigned long nodeCount = strtoul(p.value.c_str(), NULL, 0);
	if (nodeCount == 0) return nullptr;

	rc = parameters.FindParameterByName("EdgeCount", p);
	if (!rc) return nullptr;

	unsigned long edgeCount = strtoul(p.value.c_str(), NULL, 0);
	if (edgeCount == 0) return nullptr;

	rc = parameters.FindParameterByName("RootNodeID", p);
	if (!rc) return nullptr;

	unsigned long rootNodeID = strtoul(p.value.c_str(), NULL, 0);
	if (edgeCount == 0) return nullptr;

	unsigned int nodeID = 0ul;
	Vector3D position = Vector3D();
	std::vector<std::pair<unsigned int/*id*/, std::pair<bool /*has goal*/, Vector3D /*position*/>>> nodes;
	for (unsigned long i = 0; i < nodeCount; i++)
	{
		rc = parameters.FindParameterByName("Node" + std::to_string(i), p);
		if (!rc) continue;
		nodeID = strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("NodePosition" + std::to_string(i), p);
		if (!rc) continue;
		position = StringHelpers::StrToVec3(p.value);

		bool hasGoal = false;
		rc = parameters.FindParameterByName("NodeGoal" + std::to_string(i), p);
		if (rc) hasGoal = strtoul(p.value.c_str(), NULL, 0) != 0;

		nodes.push_back(std::pair<unsigned int, std::pair<bool, Vector3D>>(nodeID, std::pair<bool, Vector3D>(hasGoal, position)));
	}

	if (nodes.empty()) return nullptr;

	unsigned int parentID = 0ul, childID = 0ul, weight = 0ul;
	std::vector<std::pair<unsigned int/*weight*/, std::pair<unsigned int/*parent id*/, unsigned int/*child id*/>>> edges;
	for (unsigned long i = 0; i < edgeCount; i++)
	{
		rc = parameters.FindParameterByName("EdgeParentID" + std::to_string(i), p);
		if (!rc) continue;
		parentID = strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("EdgeChildID" + std::to_string(i), p);
		if (!rc) continue;
		childID = strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("EdgeWeight" + std::to_string(i), p);
		if (!rc) continue;
		weight = strtoul(p.value.c_str(), NULL, 0);

		edges.push_back(std::pair<unsigned int, std::pair<unsigned int, unsigned int>>(weight, std::pair<unsigned int, unsigned int>(parentID, childID)));
	}

	if (edges.empty()) return nullptr;

	Graph::Node* rootNode = nullptr;
	Graph* graph = new Graph();
	for (auto pair : nodes)
	{
		Graph::Node* node = new Graph::Node();
		node->SetID(pair.first);
		node->SetIfHasGoal(pair.second.first);
		node->SetPosition(pair.second.second);
		graph->AddNode(node);

		if (node->GetID() == rootNodeID)
		{
			rootNode = node;
		}
	}

	for (auto element : edges)
	{
		Graph::Node* parentNode = graph->FindNodeByID(element.second.first);
		Graph::Node* childNode = graph->FindNodeByID(element.second.second);
		graph->AddEdge(parentNode, childNode, (float)element.first);
	}

	PathFindingDjikstra* ai = new PathFindingDjikstra();
	ai->SetName(name);
	ai->SetGraph(graph);
	ai->Activate(isActive);
	ai->AddParent(parent);
	ai->SetRootNode(rootNode);
	if (nodeRadius != 0.0f) ai->SetNodeRadius(nodeRadius);
	if (topSpeed != 0.0f) ai->SetTopSpeed(topSpeed);

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateAStarSearch(ParameterContainer& parameters)
{
	Parameter p;
	bool rc = false;

	std::string name;
	rc = parameters.FindParameterByName("Name", p);
	rc ? name = p.value : name = StringHelpers::GenerateRandomString(4);

	rc = parameters.FindParameterByName("Parent", p);
	if (!rc) return nullptr;
	Entity* parent = (Entity*)p.valuePtr;

	bool isActive = false;
	rc = parameters.FindParameterByName("IsActive", p);
	if (rc) isActive = strtoul(p.value.c_str(), NULL, 0) != 0;

	float topSpeed = 0.0f;
	rc = parameters.FindParameterByName("TopSpeed", p);
	if (rc) topSpeed = strtof(p.value.c_str(), NULL);

	float nodeRadius = 0.0f;
	rc = parameters.FindParameterByName("NodeRadius", p);
	if (rc) nodeRadius = strtof(p.value.c_str(), NULL);

	rc = parameters.FindParameterByName("NodeCount", p);
	if (!rc) return nullptr;

	unsigned long nodeCount = strtoul(p.value.c_str(), NULL, 0);
	if (nodeCount == 0) return nullptr;

	rc = parameters.FindParameterByName("EdgeCount", p);
	if (!rc) return nullptr;

	unsigned long edgeCount = strtoul(p.value.c_str(), NULL, 0);
	if (edgeCount == 0) return nullptr;

	rc = parameters.FindParameterByName("RootNodeID", p);
	if (!rc) return nullptr;

	unsigned long rootNodeID = strtoul(p.value.c_str(), NULL, 0);
	if (rootNodeID == 0) return nullptr;

	rc = parameters.FindParameterByName("TargetNodeID", p);
	if (!rc) return nullptr;

	unsigned long targetNodeID = strtoul(p.value.c_str(), NULL, 0);
	if (targetNodeID == 0) return nullptr;

	unsigned int nodeID = 0ul;
	Vector3D position = Vector3D();
	std::vector<std::pair<unsigned int/*id*/, std::pair<bool /*has goal*/, Vector3D /*position*/>>> nodes;
	for (unsigned long i = 0; i < nodeCount; i++)
	{
		rc = parameters.FindParameterByName("Node" + std::to_string(i), p);
		if (!rc) continue;
		nodeID = strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("NodePosition" + std::to_string(i), p);
		if (!rc) continue;
		position = StringHelpers::StrToVec3(p.value);

		bool hasGoal = false;
		rc = parameters.FindParameterByName("NodeGoal" + std::to_string(i), p);
		if (rc) hasGoal = strtoul(p.value.c_str(), NULL, 0) != 0;

		nodes.push_back(std::pair<unsigned int, std::pair<bool, Vector3D>>(nodeID, std::pair<bool, Vector3D>(hasGoal, position)));
	}

	if (nodes.empty()) return nullptr;

	unsigned int parentID = 0ul, childID = 0ul, weight = 0ul;
	std::vector<std::pair<unsigned int/*weight*/, std::pair<unsigned int/*parent id*/, unsigned int/*child id*/>>> edges;
	for (unsigned long i = 0; i < edgeCount; i++)
	{
		rc = parameters.FindParameterByName("EdgeParentID" + std::to_string(i), p);
		if (!rc) continue;
		parentID = strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("EdgeChildID" + std::to_string(i), p);
		if (!rc) continue;
		childID = strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("EdgeWeight" + std::to_string(i), p);
		if (!rc) continue;
		weight = strtoul(p.value.c_str(), NULL, 0);

		edges.push_back(std::pair<unsigned int, std::pair<unsigned int, unsigned int>>(weight, std::pair<unsigned int, unsigned int>(parentID, childID)));
	}

	if (edges.empty()) return nullptr;

	Graph::Node* rootNode = nullptr, *targetNode = nullptr;
	Graph* graph = new Graph();
	for (auto pair : nodes)
	{
		Graph::Node* node = new Graph::Node();
		node->SetID(pair.first);
		node->SetIfHasGoal(pair.second.first);
		node->SetPosition(pair.second.second);
		graph->AddNode(node);

		if (node->GetID() == rootNodeID)
		{
			rootNode = node;
		}
		else if (node->GetID() == targetNodeID)
		{
			targetNode = node;
		}
		else
		{
			//Nothing
		}
	}

	for (auto element : edges)
	{
		Graph::Node* parentNode = graph->FindNodeByID(element.second.first);
		Graph::Node* childNode = graph->FindNodeByID(element.second.second);
		graph->AddEdge(parentNode, childNode, (float)element.first);
	}

	PathFindingAStar* ai = new PathFindingAStar();
	ai->SetName(name);
	ai->SetGraph(graph);
	ai->Activate(isActive);
	ai->AddParent(parent);
	ai->SetRootNode(rootNode);
	ai->SetTargetNode(targetNode);
	if (nodeRadius != 0.0f) ai->SetNodeRadius(nodeRadius);
	if (topSpeed != 0.0f) ai->SetTopSpeed(topSpeed);

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateAxisRotation(ParameterContainer& parameters)
{
	Parameter temp;

	if (!parameters.FindParameterByName("Parent", temp)) return nullptr;
	Entity* parent = (Entity*)temp.valuePtr;

	if (!parameters.FindParameterByName("VelocityGain", temp)) return nullptr;
	float velocityGain = strtof(temp.value.c_str(), NULL);

	bool isActive = true;
	if (parameters.FindParameterByName("IsActive", temp)) isActive = strtoul(temp.value.c_str(), NULL, 0) != 0;

	AxisRotation* ai = new AxisRotation();
	ai->AddParent(parent);
	ai->Activate(isActive);
	ai->SetVelocityGain(velocityGain);

	return ai;
}

IAIBehavior* AIBehaviorFactory::CreateShootAt(ParameterContainer& parameters)
{
	Parameter p;

	std::string name;
	if (parameters.FindParameterByName("Name", p)) name = p.value;

	bool isActive = true;
	if (parameters.FindParameterByName("IsActive", p)) isActive = strtoul(p.value.c_str(), NULL, 0) != 0;

	float radius = 0.0f;
	if (parameters.FindParameterByName("Radius", p)) radius = strtof(p.value.c_str(), NULL);

	float intervalBetweenShots = 0.0f;
	if (!parameters.FindParameterByName("IntervalBetweenShots", p)) return nullptr;
	intervalBetweenShots = strtof(p.value.c_str(), NULL);
		
	float projectileImpulse = 0.0f;
	if (!parameters.FindParameterByName("ProjectileImpulse", p)) return nullptr;
	projectileImpulse = strtof(p.value.c_str(), NULL);

	size_t startPositionsCount = 0;
	if (parameters.FindParameterByName("StartPositionsCount", p))
	{
		startPositionsCount = strtoul(p.value.c_str(), NULL, 0);
		if (startPositionsCount == 0)
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
	
	Entity* parent = nullptr;
	if (!parameters.FindParameterByName("Parent", p)) return nullptr;
	parent = static_cast<Entity*>(p.valuePtr);

	Scene* scene = parent->GetParent<Scene*>();
	if (!scene) return nullptr;

	Entity* target = nullptr;
	if (!parameters.FindParameterByName("Target", p)) return nullptr;
	target = static_cast<Entity*>(p.valuePtr);

	Entity* projectile = nullptr;
	if (!parameters.FindParameterByName("Projectile", p)) return nullptr;
	projectile = static_cast<Entity*>(p.valuePtr);

	Vector3D crosshair = Vector3D(0.0f, 0.0f, 0.0f);
	if (!parameters.FindParameterByName("Crosshair", p)) return nullptr;
	crosshair = StringHelpers::StrToVec3(p.value);

	std::vector<Vector3D> points;
	for (size_t i = 0; i < startPositionsCount; i++)
	{
		if (parameters.FindParameterByName("StartPosition" + std::to_string(i), p))
		{
			points.push_back(StringHelpers::StrToVec3(p.value));
		}
	}

	if (points.empty()) return nullptr;

	ShootAt* ai = new ShootAt();
	ai->SetScene(scene);
	ai->SetName(name);
	ai->Activate(isActive);
	ai->AddParent(parent);
	ai->SetProjectile(projectile);
	ai->SetProjectileImpulse(projectileImpulse);
	ai->SetCrosshair(crosshair);	
	ai->SetRadius(radius);
	ai->SetTarget(target);
	ai->SetIntervalBetweenShots(intervalBetweenShots);
	for (Vector3D point : points) ai->AddBarrelPosition(point);

	return ai;
}

AIBehaviorFactory::AIBehaviorFactory()
{
}

AIBehaviorFactory::~AIBehaviorFactory()
{
}

IAIBehavior* AIBehaviorFactory::Create(ParameterContainer& parameters)
{
	Parameter type;

	if (parameters.FindParameterByName("Type", type))
	{
		if (type.value == "seek")
		{
			return CreateSeek(parameters);
		}
		else if (type.value == "flee")
		{
			return CreateFlee(parameters);
		}
		else if (type.value == "pursue")
		{
			return CreatePursue(parameters);
		}
		else if (type.value == "evade")
		{
			return CreateEvade(parameters);
		}
		else if (type.value == "evadebymatch")
		{
			return CreateEvadeByMatch(parameters);
		}
		else if (type.value == "wander")
		{
			return CreateWander(parameters);
		}
		else if (type.value == "lookattarget")
		{
			return CreateLookAtTarget(parameters);
		}
		else if (type.value == "lookatmovingdirection")
		{
			return CreateLookAtMovingDirection(parameters);
		}
		else if (type.value == "flocking")
		{
			return CreateFlocking(parameters);
		}
		else if (type.value == "pathfollowing")
		{
			return CreatePathFollowing(parameters);
		}
		else if (type.value == "formation")
		{
			return CreateFormation(parameters);
		}
		else if (type.value == "breadthfirstsearch")
		{
			return CreateBreadthFirstSearch(parameters);
		}
		else if (type.value == "depthfirstsearch")
		{
			return CreateDepthFirstSearch(parameters);
		}
		else if (type.value == "djikstrasearch")
		{
			return CreateDjikstraSearch(parameters);
		}
		else if (type.value == "astarsearch")
		{
			return CreateAStarSearch(parameters);
		}
		else if (type.value == "axisrotation")
		{
			return CreateAxisRotation(parameters);
		}
		else if (type.value == "shootat")
		{
			return CreateShootAt(parameters);
		}
		else
		{
			return nullptr;
		}
	}

	return nullptr;
}

const bool AIBehaviorFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return true;
}
