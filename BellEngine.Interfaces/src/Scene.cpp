#include "Scene.h"
#include <BellEngineID.h>
#include <IRigidbody.h>

Scene::Scene()
{
#ifdef __linux__ 

#elif _WIN32
	InitializeCriticalSection(&lock);
#endif

	this->isActive = true;
	this->shouldDraw = true;
	this->drawToMainBuffer = false;
	this->gravity = Vector3D(0.0f, -1.0f, 0.0f);
	this->airFriction = 1.0f;
	this->camera = nullptr;

	//
	this->windAreaRadius = this->maxWindStep = this->minWindStep = 0.0f;
	this->windAreaPosition = Vector3D(0.0f, 0.0f, 0.0f);
	this->isWindEnabled = false;
}

Scene::~Scene()
{
	Dispose();

#ifdef __linux__ 

#elif _WIN32
	DeleteCriticalSection(&lock);
#endif
}

void Scene::Activate(const bool& value)
{
	this->isActive = value;
}

const bool Scene::IsActive()
{
	return this->isActive;
}

const std::string Scene::GetType()
{
	return "Scene";
}

const bool Scene::AppendFBOReference(const std::string& fboName)
{
	if (fboName.empty())
	{
		return false;
	}

	frameBufferNames.push_back(fboName);
	return true;
}

const bool Scene::Append(Entity* entity)
{
	auto it = std::find(entities.begin(), entities.end(), entity);
	if (it == entities.end())
	{
		auto sceneAsParent = this;
		entity->AddParent(sceneAsParent);

		entities.push_back(entity);
		return true;
	}

	return false;
}

const bool Scene::Append(ILight* light)
{
	lights.push_back(light);
	return true;
}

const bool Scene::Append(IScriptedAnimation* animation)
{
	animations.push_back(animation);
	return true;
}

const bool Scene::Append(IAIBehavior* aiGroupBehavior)
{
	auto it = std::find(aiGroupBehaviors.begin(), aiGroupBehaviors.end(), aiGroupBehavior);
	if (it == aiGroupBehaviors.end())
	{
		aiGroupBehaviors.push_back(aiGroupBehavior);
		return true;
	}

	return false;
}

const bool Scene::Remove(Entity* entity, const bool& deletePtr)
{
	auto it = std::find(entities.begin(), entities.end(), entity);
	if (it != entities.end())
	{
		entities.erase(it);

		if (deletePtr)
		{
			DisposePointer(entity);
		}		

		return true;
	}

	return false;
}

const bool Scene::Remove(IAIBehavior* aiGroupBehavior, const bool& deletePtr)
{
	auto it = std::find(aiGroupBehaviors.begin(), aiGroupBehaviors.end(), aiGroupBehavior);
	if (it != aiGroupBehaviors.end())
	{
		aiGroupBehaviors.erase(it);

		if (deletePtr)
		{
			DisposePointer(aiGroupBehavior);
		}

		return true;
	}

	return false;
}

const bool Scene::Remove(ILight* light, const bool& deletePtr)
{
	std::vector<ILight*>::iterator it = std::find(lights.begin(), lights.end(), light);
	if (it != lights.end())
	{
		lights.erase(it);

		if (deletePtr)
		{
			DisposePointer(light);
		}

		return true;
	}

	return false;
}

const bool Scene::Remove(IScriptedAnimation* animation, const bool& deletePtr)
{
	std::vector<IScriptedAnimation*>::iterator it = std::find(animations.begin(), animations.end(), animation);
	if (it != animations.end())
	{
		animations.erase(it);
		
		if (deletePtr)
		{
			DisposePointer(animation);
		}

		return true;
	}

	return false;
}

const bool Scene::RemoveFBOReference(const std::string& fboName)
{
	auto it = std::find(frameBufferNames.begin(), frameBufferNames.end(), fboName);

	if (it != frameBufferNames.end())
	{
		frameBufferNames.erase(it);
		return true;
	}

	return false;
}

Entity* Scene::FindEntityByName(const std::string& name)
{
	Entity* ptr = nullptr;

	for (Entity*& obj : entities)
	{
		if (obj->GetName() == name)
		{
			ptr = obj;
			break;
		}
	}

	return ptr;
}

const bool Scene::FindEntitiesByNameMatch(const std::string& name, std::vector<Entity*> entities)
{
	for (Entity* entity : this->entities)
	{
		if (entity->GetName().rfind(name) == 0)
		{
			entities.push_back(entity);
		}
	}

	return !entities.empty();
}

ILight* Scene::FindLightByName(const std::string& name)
{
	ILight* ptr = nullptr;

	for (ILight* obj : lights)
	{
		if (obj->GetName() == name)
		{
			ptr = obj;
			break;
		}
	}

	return ptr;
}

IScriptedAnimation* Scene::FindAnimationByName(const std::string& name)
{
	IScriptedAnimation* ptr = nullptr;

	for (IScriptedAnimation* obj : animations)
	{
		if (obj->GetName() == name)
		{
			ptr = obj;
			break;
		}
	}

	return ptr;
}

void Scene::SetCamera(ICamera* camera)
{
	this->camera = camera;
}

const bool Scene::IsUsingCustomFBO()
{
	return frameBufferNames.size() != 0;
}

void Scene::Dispose()
{
	DisposeVector(lights);
	DisposeVector(entities);
	DisposeVector(animations);
	DisposeVector(aiGroupBehaviors);
	DisposePointer(camera);
}

ICamera* Scene::GetCamera()
{
	return camera;
}

const bool Scene::GetEntityInCameraRange(const float& radius, std::map<float, Entity*> objects, const bool& ignoreSkybox)
{
	bool isCameraLockedAt = camera->GetTypeID() == ENGINE_OBJ_ID_GPH_CAMERALOCKEDAT;
	IRigidbody* iRigidbody = nullptr;
	float distanceFromEye = FLT_MAX;
	for (Entity*& entity : entities)
	{		
		/*Ignores skybox*/
		if (ignoreSkybox && entity->GetTypeID() == ENGINE_OBJ_ID_SCN_SKYBOX) continue;
		/*If the camera is a LockedAt, ignore its target*/
		if (isCameraLockedAt && entity->GetName() == camera->GetTargetName()) continue;
		/*Checks if the object is within the radius from the camera;*/
		iRigidbody = entity->GetComponent<IRigidbody*>();
		if (iRigidbody)
		{
			distanceFromEye = glm::distance(iRigidbody->GetPosition(), camera->GetEyePosition());
			if (distanceFromEye <= radius)
			{
				/*If it is, add to the map.*/
				objects[distanceFromEye] = entity;
			}
		}
	}

	return !objects.empty();
}

std::vector<ILight*>* Scene::GetLights()
{
	return &lights;
}

std::vector<IScriptedAnimation*>* Scene::GetAnimations()
{
	return &animations;
}

std::vector<std::string>* Scene::GetFrameBufferNames()
{
	return &frameBufferNames;
}

std::vector<IAIBehavior*>* Scene::GetAIGroupBehaviors()
{
	return &aiGroupBehaviors;
}

std::vector<Entity*>* Scene::GetEntities()
{
	return &this->entities;
}

const bool Scene::GetEntities(std::vector<Entity*>& entities)
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	entities = this->entities;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return !entities.empty();
}

void Scene::SetDrawToMainBuffer(const bool& option)
{
	drawToMainBuffer = option;
}

const bool  Scene::DrawToMainBuffer()
{
	return drawToMainBuffer;
}

void Scene::SetShouldDraw(const bool& option)
{
	this->shouldDraw = option;
}

const bool Scene::ShouldDraw()
{
	return shouldDraw;
}

void Scene::SetWindData(const bool& isActive, const float& radius, const Vector3D& position, const float& maxWindStep, const float& minWindStep)
{
	this->isWindEnabled = isActive;
	this->windAreaPosition = position;
	this->windAreaRadius = radius;
	this->maxWindStep = maxWindStep;
	this->minWindStep = minWindStep;
}

void Scene::GetWindData(float& radius, Vector3D& position, float& maxWindStep, float& minWindStep, bool& isEnabled)
{
	position = this->windAreaPosition;
	radius = this->windAreaRadius;
	maxWindStep = this->maxWindStep;
	minWindStep = this->minWindStep;
	isEnabled = this->isWindEnabled;
}

const unsigned long long Scene::GetTypeID()
{
	return ENGINE_OBJ_ID_SCN_SCENE;
}

void Scene::SetGravity(const Vector3D& gravity)
{
	this->gravity = gravity;
}

const Vector3D Scene::GetGravity()
{
	return this->gravity;
}

void Scene::SetAirFriction(const float& airFriction)
{
	this->airFriction = airFriction;
}

const float Scene::GetAirFriction()
{
	return airFriction;
}

ICloneable* Scene::Clone()
{
	Scene* clone = new Scene();

	
	return clone;
}

const std::string Scene::GetName()
{
	return this->name;
}

void Scene::SetName(const std::string& name)
{
	this->name = name;
}