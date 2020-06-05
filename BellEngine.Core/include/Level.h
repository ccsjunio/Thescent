/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <map>
#include <ILight.h>
#include <Script.h>
#include <Animation.h>
#include <Shader.h>
#include <IIdentifiable.h>
#include <Scene.h>
#include <IDisposable.h>
#include <IGraphicsBuffer.h>

class Level : public IIdentifiable, public IDisposable
{
private:
	std::vector<Scene*> scenes;
	std::vector<IGraphicsBuffer*> framebuffers;
	std::string name;

public:
	unsigned int id;
	
	Level();
	~Level();

	const std::string GetType();

	IGraphicsBuffer* FindGraphicsBufferByName(const std::string& name);
	Scene* FindSceneByName(std::string name);	
	ILight* FindLightByName(std::string name, std::string sceneName);
	Animation* FindAnimationByName(std::string name, std::string sceneName);
	Entity* FindEntityByName(const std::string& name, const std::string& sceneName);
		
	bool Append(Scene* scene);	
	const bool Append(IGraphicsBuffer* frameBuffer);

	const bool Destroy(IGraphicsBuffer* frameBuffer);
	bool Destroy(Scene* scene);

	std::vector<Scene*>* GetScenes();
	std::vector<IGraphicsBuffer*>* GetFrameBuffers();
	const unsigned long long GetTypeID();
	const std::string GetName();
	void SetName(const std::string& name);
	void Dispose();
};

