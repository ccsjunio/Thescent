#pragma once
#include <ISkinnedMesh.h>
#include <GameSystemVideo.h>
#include <Scene.h>
#include <map>
#include <ISystem.h>

class GameVertexAnimationSystem : public ISystem
{
private:
	struct AnimationInfo_s
	{
	public:
		Entity* gameObject;
		ISkinnedMesh* skinnedMesh;
		double animationTime;
		std::string nextAnimation;
	};
	std::map<std::string, AnimationInfo_s> animations;
	bool isActive, isUpdateComplete;

	void TriggerAnimation(const std::string& gameobjectName, const std::string& animationName);
	void PutCharacterToIdle(AnimationInfo_s& info);
	void ProcessAnimation(AnimationInfo_s& info);

public:
	enum class Action_e
	{
		Idle,
		Walk,
		WalkBackwards,
		Run,
		Jump,
		Action,
		StrafeRight,
		StrafeLeft,
		RollRight,
		RollLeft,
		RotateRight,
		RotateLeft
	};
	
	GameVertexAnimationSystem();
	~GameVertexAnimationSystem();

	const bool IsUpdateComplete();
	const bool IsActive();
	void Activate(const bool& value = true);
	const bool IsAnimatedCharacter(Entity* gameobject);
	void Initialize(Scene* scene);	
	void TriggerAnimation(const std::string& gameobjectName, Action_e action);
	void Update(const FrameTime& deltaTime);
};

