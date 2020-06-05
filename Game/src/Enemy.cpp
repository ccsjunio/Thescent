#include "Enemy.h"
#include <ISoundComponent.h>

void Enemy::OnShotFired(const IAIBehavior::AIEvent_s& info)
{
	//printf("PEW PEW PEW|\n");
	Entity* parent = info.ai->GetParent<Entity*>();
	if (parent && !parent->IsDisposing())
	{
		IComponent* weaponSound = parent->GetComponentByName("WeaponSound");
		if (weaponSound) weaponSound->Activate(true);
	}
}

Enemy::Enemy(const std::string& name, const float& weaponDamage, const float& health)
{
	this->name = name;
	this->weaponDamage = weaponDamage;
	this->health = health;
	this->entity.store(nullptr);
}

Enemy::~Enemy()
{
}

void Enemy::SetEntity(Entity* entity)
{
	this->entity.store(entity);

	if (entity)
	{
		auto c = entity->GetComponentByTypeName("shootat");
		if (c)
		{
			IAIBehavior* aishoot = dynamic_cast<IAIBehavior*>(c);
			if (aishoot)
			{
				aishoot->BindAIEvent(Enemy::OnShotFired);
			}
		}
	}
}

Entity* Enemy::GetEntity()
{
	return entity.load();
}

void Enemy::SetPlayerPosition(const Vector3D& position)
{
	//Not used
}

const bool Enemy::IsUpdateComplete()
{
	return this->isUpdateComplete;
}

const float Enemy::GetDamage()
{
	return this->weaponDamage;
}

const float Enemy::GetHealth()
{
	return this->health;
}

const bool Enemy::IsAlive()
{
	return this->health != 0.0;
}

void Enemy::TakeDamage(const float& damage)
{
	this->health -= damage;

	if (this->health < 0.0f)
	{
		/*Enemy died*/
		this->health = 0.0f;		
	}
}

void Enemy::SetName(const std::string name)
{
	this->name = name;
}

const std::string Enemy::GetName()
{
	return this->name;
}

void Enemy::Update(const FrameTime& frameTime)
{	
	this->frameTime = frameTime;
	this->isUpdateComplete.store(false);

	//Do stuff

	this->isUpdateComplete.store(true);
}