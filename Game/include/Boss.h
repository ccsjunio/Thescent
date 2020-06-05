#pragma once
#include "IEnemy.h"
#include <ISkinnedMesh.h>
#include <IUpdatable.h>
#include <ShaderFeature.h>

class Boss : public IEnemy
{
private:
	std::string name;
	float weaponDamage, health;
	std::atomic<Entity*> body, turretTop, turretBottom;
	std::atomic<bool> isActive, isUpdateComplete, isVisible;
	Vector3D lastPlayerPosition;
	
	static void OnShotFired(const IAIBehavior::AIEvent_s& info);
	void Hide();
	void Show();
public:
	
	Boss(const std::string& name, const float& weaponDamage, const float& health);
	~Boss();
	
	void ToggleVisibility(const bool& isVisible);
	void SetPlayerPosition(const Vector3D& position);
	void SetEntity(Entity* body, Entity* turretTop, Entity* turretBottom);
	Entity* GetEntity(const std::string& name);
	const float GetDamage();
	const float GetHealth();
	const bool IsAlive();
	void TakeDamage(const float& damage);
	void SetName(const std::string name);
	const std::string GetName();
	const bool IsUpdateComplete();
	const bool IsActive();
	void Activate(const bool& value = true);
	void Update(const FrameTime& deltaTime);
};

