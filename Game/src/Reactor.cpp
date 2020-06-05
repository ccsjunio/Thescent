#include "Reactor.h"
#include <IRender.h>
#include <IRigidbody.h>
#include <IParticleEmitter.h>

constexpr auto EXPLOSION_TIME = 5.0;

Reactor::Reactor()
{
}

Reactor::Reactor(const float& health)
{
	this->isActive.store(true);
	this->health.store(health);
	this->canTakeDamage.store(false);
}

Reactor::~Reactor()
{
}

void Reactor::AllowDamage(const bool& value)
{
	this->canTakeDamage.store(value);
}

const bool Reactor::CanTakeDamage()
{
	return this->canTakeDamage.load();
}

const float Reactor::GetHealth()
{
	return this->health.load();
}

void Reactor::SetEntity(Entity* entity)
{
	this->entity.store(entity);
}

Entity* Reactor::GetEntity()
{
	return this->entity.load();
}

void Reactor::TakeDamage(const float& damage)
{
	if (this->canTakeDamage.load())
	{
		/*Updates current health*/
		this->health.store(this->health.load() - damage);

		/*Don't allow negative health.*/
		if (this->health.load() < 0.0f)
		{
			this->health.store(0.0f);
		}

		/*if the boss died, do not take damage anymore.*/
		if (!IsAlive())
		{
			this->canTakeDamage.store(false);
		}
	}
}

const bool Reactor::IsAlive()
{
	return this->health.load() != 0.0f;
}

void Reactor::Update(const FrameTime& frameTime)
{
	if (!this->isActive.load()) return;
	this->frameTime = frameTime;

	if (!IsAlive())
	{
		/*If the reactor explodes, animates the glass of the capsule over 5 seconds*/
		if (explosionTimeAcc.load() < EXPLOSION_TIME)
		{
			Entity* e = entity.load();
			if (e)
			{
				/*Create visual FX usign the shader.*/
				IRender* vr = e->GetComponent<IRender*>();
				if (vr)
				{					
					/*Increases the red color over time, until the end of the shader animation*/
					Vector4D color = vr->GetDiffuse();
					color.r += this->frameTime.GetRunTime() / 1000;
					vr->SetDiffuse(color);
				}
			}			

			/*Accumulates time*/
			explosionTimeAcc.store(explosionTimeAcc.load() + this->frameTime.GetDeltaTime());
		}
		else
		{
			/*Disable entity*/
			Entity* e = entity.load();
			if (e)
			{
				/*Disable collision detection*/
				IRigidbody* rb = e->GetComponent<IRigidbody*>();
				if (rb)
				{
					rb->ToggleCollisionDetection(false);
				}
				/*Make it not visible*/
				IRender* vr = e->GetComponent<IRender*>();
				if (vr)
				{
					vr->SetVisibility(false);
				}
			}

			//Deactivate updates
			this->isActive.store(true);
		}	
	}
}
