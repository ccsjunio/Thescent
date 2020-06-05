#include "Hostage.h"
#include <IRigidbody.h>

#define ANIMATION_IDLE		"Idle"
#define ANIMATION_RUNNING	"Running"
#define ANIMATION_STRUGGLE	"Attack"

void Hostage::AIController()
{
	

	IRigidbody* bossRB = vertexAnimation->entity.load()->GetComponent<IRigidbody*>();
	if (bossRB)
	{
		float distanceToPlayer = glm::distance(bossRB->GetPosition(), lastPlayerPosition);

		if (distanceToPlayer > 40.0f)
		{
			if (vertexAnimation->skinnedMesh->GetCurrentAnimation() != ANIMATION_STRUGGLE)
			{
				TriggerAnimation(ANIMATION_STRUGGLE);		
			}
		}
		else
		{
			if (vertexAnimation->skinnedMesh->GetCurrentAnimation() != ANIMATION_RUNNING)
			{
				TriggerAnimation(ANIMATION_RUNNING);				
			}
		}
	}
}

Hostage::Hostage()
{
	this->lastPlayerPosition = Vector3D(0.0f, 0.0f, 0.0f);
	this->isActive.store(true);
	this->wasRescued.store(false);
	this->isUpdateComplete.store(false);
	this->vertexAnimation = new Hostage::AnimationInfo_s();
}

Hostage::~Hostage()
{
	if (vertexAnimation)
	{
		delete (vertexAnimation);
		vertexAnimation = nullptr;
	}
}

const bool Hostage::IsUpdateComplete()
{
	return this->isUpdateComplete.load();
}

const bool Hostage::IsActive()
{
	return this->isActive.load();
}

void Hostage::Activate(const bool& value)
{
	this->isActive.store(value);
}

void Hostage::SetPlayerPosition(const Vector3D& position)
{
	this->lastPlayerPosition = position;
}

void Hostage::SetEntity(Entity* entity)
{
	if (entity)
	{
		vertexAnimation->animationTime = 0.0f;
		vertexAnimation->entity.store(entity);
		vertexAnimation->skinnedMesh = vertexAnimation->entity.load()->GetComponent<ISkinnedMesh*>();
		vertexAnimation->nextAnimation = ANIMATION_IDLE;
	}
}

Entity* Hostage::GetEntity()
{
	return vertexAnimation->entity.load();
}

void Hostage::SetName(const std::string name)
{
	this->name = name;
}

const std::string Hostage::GetName()
{
	return this->name;
}

void Hostage::PutCharacterToIdle()
{
	if (vertexAnimation->skinnedMesh)
	{
		vertexAnimation->nextAnimation = ANIMATION_IDLE;
		vertexAnimation->animationTime = 0.0f;
	}
	else
	{
		printf("Could not find character skinnedmesh\n");
	}
}

void Hostage::ProcessAnimation()
{
	//No current animation? Set to idle.
	if (vertexAnimation->skinnedMesh->GetCurrentAnimation().empty())
	{
		PutCharacterToIdle();
		vertexAnimation->skinnedMesh->SetCurrentAnimation(vertexAnimation->nextAnimation);
	}
	/*Update current animation*/
	vertexAnimation->skinnedMesh->PlayAnimation(vertexAnimation->animationTime/*0.0f- Frame time*/, vertexAnimation->skinnedMesh->GetCurrentAnimation());
	/*Checks if we can go to the next animation or not*/
	if (vertexAnimation->skinnedMesh->GetCurrentAnimation() != ANIMATION_IDLE)
	{
		/*If animation ended, go to next animation*/
		if (vertexAnimation->animationTime > vertexAnimation->skinnedMesh->GetDuration())
		{
			vertexAnimation->animationTime = 0.0f;
			vertexAnimation->skinnedMesh->SetCurrentAnimation(vertexAnimation->nextAnimation);
		}
		else
		{
			//Animation is still running
			//Just increment animation time
		}
	}
	else
	{
		/*If the next animation is NOT idle, then override Idle*/
		if (vertexAnimation->nextAnimation != ANIMATION_IDLE)
		{
			/*Don't wait for idle to complete*/
			vertexAnimation->animationTime = 0.0f;
			vertexAnimation->skinnedMesh->SetCurrentAnimation(vertexAnimation->nextAnimation);
		}
		/*If the next animation is idle, then wait*/
		else
		{
			/*If animation ended, go to next animation*/
			if (vertexAnimation->animationTime > vertexAnimation->skinnedMesh->GetDuration())
			{
				vertexAnimation->animationTime = 0.0f;
				vertexAnimation->skinnedMesh->SetCurrentAnimation(vertexAnimation->nextAnimation);
			}
			else
			{
				//Animation is still running
				//Just increment animation time
			}
		}
	}

	vertexAnimation->animationTime += frameTime.GetDeltaTime();
}

void Hostage::TriggerAnimation(const std::string& animationName)
{
	vertexAnimation->nextAnimation = animationName;
}

void Hostage::TriggerAnimation(Hostage::State_e action)
{
	switch (action)
	{
	case Hostage::State_e::Idle:
	{
		TriggerAnimation(ANIMATION_IDLE);
	}
	break;
	case Hostage::State_e::Waving:
	{
		TriggerAnimation(ANIMATION_RUNNING);
	}
	break;
	case Hostage::State_e::Attack:
	{
		TriggerAnimation(ANIMATION_STRUGGLE);
	}
	break;
	case Hostage::State_e::None:
	default:
		break;
	}
}

const bool Hostage::WasRescued()
{
	return this->wasRescued.load();
}

void Hostage::Rescue()
{
	this->wasRescued.store(true);
}

void Hostage::Update(const FrameTime& frameTime)
{
	if (!this->isActive.load()) return;
	this->isUpdateComplete.store(false);

	SetFrameTime(frameTime);

	/*Hostage was rescued, stop updating state machine*/
	if (this->vertexAnimation->entity.load() && !this->vertexAnimation->entity.load()->IsDisposing() && !WasRescued())
	{
		/*State machine*/
		AIController();

		ProcessAnimation();
	}
	else
	{
		this->isActive.store(false);
	}

	this->isUpdateComplete.store(true);
}
