#pragma once
#include <atomic>
#include <Entity.h>
#include <FrameTime.h>

class Reactor
{
private:
	FrameTime frameTime;
	std::atomic<bool> isActive, canTakeDamage;
	std::atomic<float> health;
	std::atomic<Entity*> entity;
	std::atomic<float> explosionTimeAcc;

public:
	Reactor();
	Reactor(const float& health);
	~Reactor();

	void AllowDamage(const bool& value);
	const bool CanTakeDamage();
	const float GetHealth();
	void SetEntity(Entity* entity);
	Entity* GetEntity();
	void TakeDamage(const float& damage);
	const bool IsAlive();
	void Update(const FrameTime& frameTime);
};

