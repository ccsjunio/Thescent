#include "GameVertexAnimationSystem.h"
#include <EngineFolders.h>
#include <IRigidbody.h>

GameVertexAnimationSystem::GameVertexAnimationSystem()
{
	this->isActive = true;
	frameTime = FrameTime();
}
GameVertexAnimationSystem::~GameVertexAnimationSystem()
{
}

const bool GameVertexAnimationSystem::IsUpdateComplete()
{
	return this->isUpdateComplete;
}

const bool GameVertexAnimationSystem::IsActive()
{
	return this->isActive;
}

void GameVertexAnimationSystem::Activate(const bool& value)
{
	this->isActive = value;
}

void GameVertexAnimationSystem::Initialize(Scene* scene)
{
	std::vector<Entity*> entities;
	if (scene->GetEntities(entities))
	{
		for (Entity*& entity : entities)
		{
			if (entity->HasComponent<ISkinnedMesh*>())
			{
				AnimationInfo_s info;
				info.animationTime = 0.0f;
				info.gameObject = entity;
				info.skinnedMesh = entity->GetComponent<ISkinnedMesh*>();
				info.nextAnimation = "Idle";

				animations[entity->GetName()] = info;
			}
		}
	}
}

const bool GameVertexAnimationSystem::IsAnimatedCharacter(Entity* gameobject)
{
	return gameobject->HasComponent<ISkinnedMesh*>();
}

void GameVertexAnimationSystem::PutCharacterToIdle(AnimationInfo_s& info)
{
	if (info.skinnedMesh)
	{
		info.nextAnimation = "Idle";
		info.animationTime = 0.0f;
	}
	else
	{
		printf("Could not find character skinnedmesh\n");
	}
}

void GameVertexAnimationSystem::ProcessAnimation(AnimationInfo_s& info)
{
	//No current animation? Set to idle.
	if (info.skinnedMesh->GetCurrentAnimation().empty())
	{
		PutCharacterToIdle(info);
		info.skinnedMesh->SetCurrentAnimation(info.nextAnimation);
	}
	/*Update current animation*/
	info.skinnedMesh->PlayAnimation(info.animationTime/*0.0f- Frame time*/, info.skinnedMesh->GetCurrentAnimation());
	/*Checks if we can go to the next animation or not*/
	if (info.skinnedMesh->GetCurrentAnimation() != "Idle")
	{
		/*If animation ended, go to next animation*/
		if (info.animationTime > info.skinnedMesh->GetDuration())
		{
			info.animationTime = 0.0f;
			info.skinnedMesh->SetCurrentAnimation(info.nextAnimation);
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
		if (info.nextAnimation != "Idle")
		{
			/*Don't wait for idle to complete*/
			info.animationTime = 0.0f;
			info.skinnedMesh->SetCurrentAnimation(info.nextAnimation);
		}
		/*If the next animation is idle, then wait*/
		else
		{
			/*If animation ended, go to next animation*/
			if (info.animationTime > info.skinnedMesh->GetDuration())
			{
				info.animationTime = 0.0f;
				info.skinnedMesh->SetCurrentAnimation(info.nextAnimation);
			}
			else
			{
				//Animation is still running
				//Just increment animation time
			}
		}
	}

	info.animationTime += frameTime.GetDeltaTime();
}

void GameVertexAnimationSystem::TriggerAnimation(const std::string& gameobjectName, const std::string& animationName)
{
	animations[gameobjectName].nextAnimation = animationName;
}

void GameVertexAnimationSystem::Update(const FrameTime& frameTime)
{
	if (!this->isActive) return;
	this->isUpdateComplete = false;

	SetFrameTime(frameTime);

	for (auto element : animations)
	{
		ProcessAnimation(animations[element.first]);
	}

	this->isUpdateComplete = true;
}

void GameVertexAnimationSystem::TriggerAnimation(const std::string& gameobjectName, Action_e action)
{
	Entity* go = animations[gameobjectName].gameObject;

	if (!go)
	{
		//Error, object not found
		return;
	}

	if (!IsAnimatedCharacter(go))
	{
		//Not an animated character
		return;
	}
	
	IRigidbody* rb = go->GetComponent<IRigidbody*>();
	if (rb)
	{
		Vector3D orientation = rb->GetOrientation();
		float moveForce = 0.3f;

		switch (action)
		{
		case GameVertexAnimationSystem::Action_e::Idle:
		{
			rb->ClampVelocity();
			TriggerAnimation(go->GetName(), "Idle");
		}
		break;
		case GameVertexAnimationSystem::Action_e::Walk:
		{
			rb->MoveFB(orientation.z < 0.0f ? -moveForce : moveForce, this->frameTime.GetDeltaTime());
			TriggerAnimation(go->GetName(), "Walk");
		}
		break;
		case GameVertexAnimationSystem::Action_e::WalkBackwards:
		{
			rb->MoveFB(orientation.z < 0.0f ? moveForce : -moveForce, this->frameTime.GetDeltaTime());
			TriggerAnimation(go->GetName(), "WalkBackwards");
		}
		break;
		case GameVertexAnimationSystem::Action_e::Run:
		{
			rb->MoveFB(orientation.z < 0.0f ? 2 * -moveForce : 2 * moveForce, this->frameTime.GetDeltaTime());
			TriggerAnimation(go->GetName(), "Run");
		}
			break;
		case GameVertexAnimationSystem::Action_e::Jump:
		{
			//rb->ApplyForce(Vector3D(0.0f, 1.0f, 0.0f));
			TriggerAnimation(go->GetName(), "Jump");
		}
		break;
		case GameVertexAnimationSystem::Action_e::Action:
		{
			TriggerAnimation(go->GetName(), "Action");
		}
			break;
		case GameVertexAnimationSystem::Action_e::StrafeRight:
		{
			rb->MoveRL(orientation.z < 0.0f ? -moveForce : moveForce, this->frameTime.GetDeltaTime());
			TriggerAnimation(go->GetName(), "StrafeRight");
		}
		break;
		case GameVertexAnimationSystem::Action_e::StrafeLeft:
		{
			rb->MoveRL(orientation.z < 0.0f ? moveForce : -moveForce, this->frameTime.GetDeltaTime());
			TriggerAnimation(go->GetName(), "StrafeLeft");
		}
		break;
		case GameVertexAnimationSystem::Action_e::RollRight:
		{
			rb->MoveRL(orientation.z < 0.0f ? 2 *-moveForce : 2* moveForce, this->frameTime.GetDeltaTime());
			TriggerAnimation(go->GetName(), "RollRight");
		}
		break;
		case GameVertexAnimationSystem::Action_e::RollLeft:
		{
			rb->MoveRL(orientation.z < 0.0f ? 2 * moveForce : 2 * -moveForce, this->frameTime.GetDeltaTime());
			TriggerAnimation(go->GetName(), "RollLeft");
		}
		break;
		case GameVertexAnimationSystem::Action_e::RotateRight:
		{
			Vector3D currRot = rb->GetRotation_Euler();
			currRot.y -= 1.0f;
			rb->SetRotation(currRot);
		}
		break;
		case GameVertexAnimationSystem::Action_e::RotateLeft:
		{
			Vector3D currRot = rb->GetRotation_Euler();
			currRot.y += 1.0f;
			rb->SetRotation(currRot);
		}
		break;
		default:
			break;
		}
	}
}