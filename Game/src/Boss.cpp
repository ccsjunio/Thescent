#include "Boss.h"
#include <ISoundComponent.h>
#include <IRigidbody.h>
#include <Easing.h>
#include <IRender.h>

void Boss::OnShotFired(const IAIBehavior::AIEvent_s& info)
{
	//Play sound FX
}

void Boss::Hide()
{
	//Hide top
	{
		IRender* vr =  turretTop.load()->GetComponent<IRender*>();
		if (vr)
		{
			ShaderFeature* colorize = vr->GetShaderFeature("COLORIZE");
			colorize->Activate(true);

			ShaderFeature* interpolation_colorize = vr->GetShaderFeature("INTERPOLATION_COLORIZE");
			interpolation_colorize->Activate(false);
		}
	}

	//Hide bottom
	{
		IRender* vr = turretBottom.load()->GetComponent<IRender*>();
		if (vr)
		{
			ShaderFeature* colorize = vr->GetShaderFeature("COLORIZE");
			colorize->Activate(true);

			ShaderFeature* interpolation_colorize = vr->GetShaderFeature("INTERPOLATION_COLORIZE");
			interpolation_colorize->Activate(false);
		}
	}

	//Hide body
	{
		IRender* vr = body.load()->GetComponent<IRender*>();
		if (vr)
		{
			ShaderFeature* colorize = vr->GetShaderFeature("COLORIZE");
			colorize->Activate(true);

			ShaderFeature* interpolation_colorize = vr->GetShaderFeature("INTERPOLATION_COLORIZE");
			interpolation_colorize->Activate(false);
		}
	}
}

void Boss::Show()
{
	//Hide top
	{
		IRender* vr = turretTop.load()->GetComponent<IRender*>();
		if (vr)
		{
			ShaderFeature* colorize = vr->GetShaderFeature("COLORIZE");
			colorize->Activate(false);

			ShaderFeature* interpolation_colorize = vr->GetShaderFeature("INTERPOLATION_COLORIZE");
			interpolation_colorize->Activate(true);
		}
	}

	//Hide bottom
	{
		IRender* vr = turretBottom.load()->GetComponent<IRender*>();
		if (vr)
		{
			ShaderFeature* colorize = vr->GetShaderFeature("COLORIZE");
			colorize->Activate(false);

			ShaderFeature* interpolation_colorize = vr->GetShaderFeature("INTERPOLATION_COLORIZE");
			interpolation_colorize->Activate(true);
		}
	}

	//Hide body
	{
		IRender* vr = body.load()->GetComponent<IRender*>();
		if (vr)
		{
			ShaderFeature* colorize = vr->GetShaderFeature("COLORIZE");
			colorize->Activate(false);

			ShaderFeature* interpolation_colorize = vr->GetShaderFeature("INTERPOLATION_COLORIZE");
			interpolation_colorize->Activate(true);
		}
	}
}

const bool Boss::IsUpdateComplete()
{
	return this->isUpdateComplete.load();
}

const bool Boss::IsActive()
{
	return this->isActive.load();
}

void Boss::Activate(const bool& value)
{
	this->isActive.store(value);
}

Boss::Boss(const std::string& name, const float& weaponDamage, const float& health)
{
	this->isActive.store(true);
	this->name = name;
	this->weaponDamage = weaponDamage;
	this->health = health;
	
	this->body.store(nullptr);
	this->turretTop.store(nullptr);
	this->turretBottom.store(nullptr);
}

Boss::~Boss()
{
	
}

void Boss::ToggleVisibility(const bool& isVisible)
{
	if (this->IsAlive())
	{
		this->isVisible.store(isVisible);
		/*Toggle visibility in the shader*/
		this->isVisible.load() ? Show() : Hide();
	}
}

void Boss::SetPlayerPosition(const Vector3D& position)
{
	this->lastPlayerPosition = position;
}

void Boss::SetEntity(Entity* body, Entity* turretTop, Entity* turretBottom)
{
	this->body.store(body);
	this->turretTop.store(turretTop);
	this->turretBottom.store(turretBottom);
}

Entity* Boss::GetEntity(const std::string& name)
{
	if(name.find("BODY") != std::string::npos)
	{
		return body.load();
	}

	else if (name.find("TOP") != std::string::npos)
	{
		return turretTop.load();
	}

	else if (name.find("BOTTOM") != std::string::npos)
	{
		return turretBottom.load();
	}

	return nullptr;
}

const float Boss::GetDamage()
{
	return this->weaponDamage;
}

const float Boss::GetHealth()
{
	return this->health;
}

const bool Boss::IsAlive()
{
	return this->health != 0.0;
}

void Boss::TakeDamage(const float& damage)
{
	this->health -= damage;

	if (this->health < 0.0f)
	{
		/*Enemy died*/
		this->health = 0.0f;
	}

	///*If the enemy died, play sound*/
	//if (!IsAlive())
	//{
	//	/*Play sound*/
	//	ISoundComponent* sc = dynamic_cast<ISoundComponent*>(entity.load()->GetComponentByName("EnemyExplosion"));
	//	if (sc) sc->Activate(true);
	//}
}

void Boss::SetName(const std::string name)
{
	this->name = name;
}

const std::string Boss::GetName()
{
	return this->name;
}

void Boss::Update(const FrameTime& frameTime)
{
	if (!this->isActive.load()) return;
	this->isUpdateComplete.store(false);

	SetFrameTime(frameTime);

	

	this->isUpdateComplete.store(true);
}
