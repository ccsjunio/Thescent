#pragma once
#include <ISystem.h>
#include <ISingleton.h>
#include <Scene.h>
#include "GameSystemVideo.h"
#include <IDisposable.h>
//FOR AI PROJECT 03
#include <ColorCode.h>
#include <ISystemAI.h>
#include <IRigidbody.h>
#include <functional>
#include <IRender.h>
#include <IParticleEmitter.h>

class GameSystemScene : public ISystem, public IDisposable
{
private:
	bool isActive, isUpdateComplete, initializedScenePtrs;

	std::function<void(const std::string&, const double&, const unsigned int&)> onExternalMessage;
	
	void GetScene01Objects(Scene* scene1);
	void CompleteScene01(Scene* scene1);
	
	Scene* currentScene, *deposit_scene;
	void UpdateScene1Rules();
public:
	GameSystemScene();
	~GameSystemScene();
		
	void InitializeScene01(Scene* scene1);
	Scene* GetDepositScene();
	void PutSphere(Entity* where);
	void BindOnExternalMessage(std::function<void(const std::string&, const double&, const unsigned int&)> fn);
	const bool IsUpdateComplete();
	const bool Initialize(ParameterContainer& input);
	const bool IsActive();
	void Activate(const bool& value = true);
	void SetScene(Scene* scene);
	void Update(const FrameTime& deltaTime);
	void Dispose();
};

