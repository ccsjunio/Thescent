#pragma once
#include <string>
#include <Entity.h>
#include <atomic>
#include <IAIBehavior.h>
#include "IEnemy.h"
#include <atomic>

class Enemy : public IEnemy
{
private:
	std::string name;
	float weaponDamage, health;
	std::atomic<Entity*> entity;
	std::atomic<bool> isUpdateComplete, isActive;
	FrameTime frameTime;
	static void OnShotFired(const IAIBehavior::AIEvent_s& info);

public:

	Enemy(const std::string& name, const float& weaponDamage, const float& health);
	~Enemy();

	void SetPlayerPosition(const Vector3D& position);
	const bool IsUpdateComplete();
	void SetEntity(Entity* entity);
	Entity* GetEntity();
	const float GetDamage();
	const float GetHealth();
	const bool IsAlive();
	void TakeDamage(const float& damage);
	void SetName(const std::string name);
	const std::string GetName();
	void Update(const FrameTime& deltaTime);
};

