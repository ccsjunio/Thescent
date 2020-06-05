#pragma once
#include <atomic>
#include <MathLibrary.h>
#include <FrameTime.h>
#include <ISkinnedMesh.h>
#include <Entity.h>
#include <IUpdatable.h>

class Hostage : public IUpdatable
{
private:
	class AnimationInfo_s
	{
	public:
		std::atomic<Entity*> entity;
		ISkinnedMesh* skinnedMesh;
		double animationTime;
		std::string nextAnimation;
	};

	Hostage::AnimationInfo_s* vertexAnimation;
	std::atomic<bool> wasRescued, isActive, isUpdateComplete;
	std::string name;
	Vector3D lastPlayerPosition;

	void AIController();
	void TriggerAnimation(const std::string& animationName);
	void PutCharacterToIdle();
	void ProcessAnimation();
public:
	enum class State_e
	{
		None,

		Idle,
		Waving,
		Attack
	};

	Hostage();
	~Hostage();

	void SetPlayerPosition(const Vector3D& position);
	void SetEntity(Entity* entity);
	Entity* GetEntity();
	void SetName(const std::string name);
	const std::string GetName();
	const bool IsUpdateComplete();
	const bool IsActive();
	void Activate(const bool& value = true);
	void TriggerAnimation(Hostage::State_e action);
	const bool WasRescued();
	void Rescue();
	void Update(const FrameTime& frameTime);
};

