#pragma once
#include <vector>
#include "ICamera.h"
#include "ILight.h"
#include <IScriptedAnimation.h>
#include "IAIBehavior.h"
#include <IScript.h>
#include <IEntity.h>
#include <Entity.h>

#ifdef __linux__ 
#include <mutex>
#elif _WIN32
#define NOMINMAX
#include <windows.h>
#else

#endif

class Scene : public IEntity
{
private:
#ifdef __linux__ 
	std::mutex mutex;
#elif _WIN32
	CRITICAL_SECTION lock;
#else

#endif

	ICamera* camera;
	std::vector<Entity*> entities;
	std::vector<ILight*> lights;
	std::vector<IScriptedAnimation*> animations;
	bool drawToMainBuffer;
	std::vector<std::string> frameBufferNames;
	std::vector<IAIBehavior*> aiGroupBehaviors;
	Vector3D gravity;
	float airFriction;
	bool shouldDraw;
	bool isActive;
	std::string name;

	/*Wind area of activity and force bounds*/
	float windAreaRadius;
	Vector3D windAreaPosition;
	float maxWindStep, minWindStep;
	bool isWindEnabled;

protected:


public:
	Scene();
	~Scene();

	void Activate(const bool& value);
	const bool IsActive();

	const std::string GetName();
	void SetName(const std::string& name);
	const std::string GetType();
	const unsigned long long GetTypeID();

	const bool AppendFBOReference(const std::string& fboName);
	const bool Append(Entity* entity);
	const bool Append(ILight* light);
	const bool Append(IScriptedAnimation* animation);
	const bool Append(IAIBehavior* aiGroupBehavior);
	const bool Remove(Entity* entity, const bool& deletePtr = true);
	const bool Remove(IAIBehavior* aiGroupBehavior, const bool& deletePtr = true);
	const bool Remove(ILight* light, const bool& deletePtr = true);
	const bool Remove(IScriptedAnimation* animation, const bool& deletePtr = true);
	const bool RemoveFBOReference(const std::string& fboName);

	Entity* FindEntityByName(const std::string& name);
	const bool FindEntitiesByNameMatch(const std::string& name, std::vector<Entity*> entities);
	ILight* FindLightByName(const std::string& name);
	IScriptedAnimation* FindAnimationByName(const std::string& name);

	ICamera* GetCamera();
	const bool GetEntityInCameraRange(const float& radius, std::map<float, Entity*> objects, const bool& ignoreSkybox = true);
	std::vector<ILight*>* GetLights();
	std::vector<IScriptedAnimation*>* GetAnimations();
	std::vector<std::string>* GetFrameBufferNames();
	std::vector<IAIBehavior*>* GetAIGroupBehaviors();
	std::vector<Entity*>* GetEntities();
	const bool GetEntities(std::vector<Entity*>& entities);

	void SetCamera(ICamera* camera);
	void SetGravity(const Vector3D& gravity);
	const Vector3D GetGravity();
	void SetAirFriction(const float& airFriction);
	const float GetAirFriction();

	void SetWindData(const bool& isActive, const float& radius, const Vector3D& position, const float& maxWindStep, const float& minWindStep);
	void GetWindData(float& radius, Vector3D& position, float& maxWindStep, float& minWindStep, bool& isEnabled);

	void SetDrawToMainBuffer(const bool& option);
	const bool DrawToMainBuffer();
	void SetShouldDraw(const bool& option);
	const bool ShouldDraw();

	ICloneable* Clone();
	const bool IsUsingCustomFBO();
	void Dispose();
};