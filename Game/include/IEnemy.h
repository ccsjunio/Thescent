#pragma once
#include <string>
#include <Entity.h>
#include <atomic>
#include <IAIBehavior.h>
#include <IUpdatable.h>
#include <MathLibrary.h>

class IEnemy : public IUpdatable
{
protected:
	//static void OnShotFired(const IAIBehavior::AIEvent_s& info);

	IEnemy() {};

public:
	virtual ~IEnemy() {};

	virtual void SetPlayerPosition(const Vector3D& position) = 0;
	//virtual void SetEntity(Entity* entity) = 0;
	//virtual Entity* GetEntity() = 0;
	virtual const float GetDamage() = 0;
	virtual const float GetHealth() = 0;
	virtual const bool IsAlive() = 0;
	virtual void TakeDamage(const float& damage) = 0;
	virtual void SetName(const std::string name) = 0;
	virtual const std::string GetName() = 0;
};

