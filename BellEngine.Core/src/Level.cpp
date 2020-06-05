/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#include "Level.h"
#include <BellEngineID.h>

Level::Level()
{
	id = 0;
}

Level::~Level()
{
	Dispose();
}

const std::string Level::GetType()
{
	return "Level";
}

IGraphicsBuffer* Level::FindGraphicsBufferByName(const std::string& name)
{
	IGraphicsBuffer* ptr = nullptr;

	for (IGraphicsBuffer* obj : framebuffers)
	{
		if (obj->GetName() == name)
		{
			ptr = obj;
			break;
		}
	}

	return ptr;
}

Scene* Level::FindSceneByName(std::string name)
{
	Scene* ptr = nullptr;

	for (Scene* obj : scenes)
	{
		if (obj->GetName() == name)
		{
			ptr = obj;
			break;
		}
	}

	return ptr;
}

ILight* Level::FindLightByName(std::string name, std::string sceneName)
{
	ILight* ptr = nullptr;

	Scene* scene = FindSceneByName(sceneName);
	if (scene)
	{
		ptr = scene->FindLightByName(name);
	}

	return ptr;
}

Animation* Level::FindAnimationByName(std::string name, std::string sceneName)
{
	Animation* ptr = nullptr;

	//Scene* scene = FindSceneByName(sceneName);
	//if (scene)
	//{
	//	ptr = scene->FindAnimationByName(name);
	//}

	return ptr;
}

Entity* Level::FindEntityByName(const std::string& name, const std::string& sceneName)
{
	Entity* ptr = nullptr;

	Scene* scene = FindSceneByName(sceneName);
	if (scene)
	{
		ptr = scene->FindEntityByName(name);
	}

	return ptr;
}

bool Level::Append(Scene* scene)
{
	if (!scene)
	{
		return false;
	}

	scenes.push_back(scene);
	return true;
}

const bool Level::Append(IGraphicsBuffer* frameBuffer)
{
	if (!frameBuffer)
	{
		return false;
	}

	framebuffers.push_back(frameBuffer);
	return true;
}

const bool Level::Destroy(IGraphicsBuffer* frameBuffer)
{
	std::vector<IGraphicsBuffer*>::iterator it = std::find(framebuffers.begin(), framebuffers.end(), frameBuffer);
	if (it != framebuffers.end())
	{
		framebuffers.erase(it);
		delete (*it);

		return true;
	}

	return false;
}

bool Level::Destroy(Scene* scene)
{
	std::vector<Scene*>::iterator it = std::find(scenes.begin(), scenes.end(), scene);
	if (it != scenes.end())
	{
		scenes.erase(it);
		delete (*it);

		return true;
	}

	return false;
}

std::vector<Scene*>* Level::GetScenes()
{
	return &scenes;
}

std::vector<IGraphicsBuffer*>* Level::GetFrameBuffers()
{
	return &framebuffers;
}

void Level::SetName(const std::string& name)
{
	this->name = name;
}

void Level::Dispose()
{
	DisposeVector(framebuffers);
	DisposeVector(scenes);
}

const unsigned long long Level::GetTypeID()
{
	return ENGINE_OBJ_ID_SCN_LEVEL;
}

const std::string Level::GetName()
{
	return name;
}
