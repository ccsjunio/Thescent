#include "pch.h"
#include "SystemAI.h"
#include <IAIBehavior.h>
#include "AIBehaviorFactory.h"

void SystemAI::Process()
{
	if (scene)
	{
		std::vector<Entity*> entities;
		if (scene->GetEntities(entities))
		{
			for (const auto& entity : entities)
			{
				if (entity->IsDisposing()) continue;

				std::vector<IAIBehavior*> aiBehaviors;
				if (entity->GetComponents(aiBehaviors))
				{
					for (const auto& behavior : aiBehaviors)
					{
						if (behavior && behavior->IsActive())
						{
							behavior->Update(frameTime);
						}
					}
				}
			}
		}

		for (const auto& behavior : *scene->GetAIGroupBehaviors())
		{
			if (behavior->IsActive())
			{
				behavior->Update(frameTime);
			}
		}
	}
}

SystemAI::SystemAI()
{
	this->isUpdateComplete = false;
	this->isActive = true;
	behaviorFactory = new AIBehaviorFactory();
}

SystemAI::~SystemAI()
{
}

const bool SystemAI::IsUpdateComplete()
{
	return this->isUpdateComplete.load();
}

const bool SystemAI::IsActive()
{
	return this->isActive.load();
}

void SystemAI::Activate(const bool& value)
{
	this->isActive.store(value);
}

void SystemAI::SetScene(Scene*& scene)
{
	this->scene = scene;
}

IFactory<IAIBehavior>* SystemAI::GetBehaviorFactory()
{
	if (!behaviorFactory)
	{
		behaviorFactory = new AIBehaviorFactory();
	}

	return behaviorFactory;
}

void SystemAI::Update(const FrameTime& frameTime)
{
	if (!this->isActive) return;
	this->isUpdateComplete.store(false);

	SetFrameTime(frameTime);

	Process();

	this->isUpdateComplete.store(true);
}