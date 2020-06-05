#pragma once
#include <ISystemAI.h>
#include <atomic>

class SystemAI : public ISystemAI
{
private:
	std::atomic<bool> isActive, isUpdateComplete;
	void Process();

public:
	SystemAI();
	~SystemAI();
	
	const bool IsUpdateComplete();
	const bool IsActive();
	void Activate(const bool& value = true);
	void SetScene(Scene*& scene);
	IFactory<IAIBehavior>* GetBehaviorFactory();
	void Update(const FrameTime& deltaTime);
};

