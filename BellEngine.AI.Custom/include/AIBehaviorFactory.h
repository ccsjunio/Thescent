#pragma once
#include <IFactory.h>
#include <IAIBehavior.h>

class AIBehaviorFactory : public IFactory<IAIBehavior>
{
private:
	IAIBehavior* CreateSeek(ParameterContainer& parameters);
	IAIBehavior* CreateFlee(ParameterContainer& parameters);
	IAIBehavior* CreatePursue(ParameterContainer& parameters);
	IAIBehavior* CreateEvade(ParameterContainer& parameters);
	IAIBehavior* CreateEvadeByMatch(ParameterContainer& parameters);
	IAIBehavior* CreateWander(ParameterContainer& parameters);
	IAIBehavior* CreateLookAtTarget(ParameterContainer& parameters);
	IAIBehavior* CreateLookAtMovingDirection(ParameterContainer& parameters);
	IAIBehavior* CreatePathFollowing(ParameterContainer& parameters);
	IAIBehavior* CreateFlocking(ParameterContainer& parameters);
	IAIBehavior* CreateFormation(ParameterContainer& parameters);
	IAIBehavior* CreateBreadthFirstSearch(ParameterContainer& parameters);
	IAIBehavior* CreateDepthFirstSearch(ParameterContainer& parameters);
	IAIBehavior* CreateDjikstraSearch(ParameterContainer& parameters);
	IAIBehavior* CreateAStarSearch(ParameterContainer& parameters);
	IAIBehavior* CreateAxisRotation(ParameterContainer& parameters);
	IAIBehavior* CreateShootAt(ParameterContainer& parameters);

public:
	AIBehaviorFactory();
	~AIBehaviorFactory();

	IAIBehavior* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
};

