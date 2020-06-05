/***********************************************************************
* Project           : Game
* Author            : Felipe Bellini
* Date created      : Oct. 12 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <string>
#include <vector>
#pragma warning(push, 0)   
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#pragma warning(pop)
#include "Level.h"
#include <ISystemSound.h>
#include <ISystemMesh.h>
#include <IFactory.h>
#include <ISystemPhysics.h>
#include <ISystemAI.h>
#include <ISystemScripting.h>
#include "ComponentFactory.h"
#include <IDisposable.h>
#include <VideoFactory.h>
#include <functional>
#include <ISystemDataPersistency.h>
#include <Entity.h>

class LevelFactory : public IFactory<Level>, public IDisposable
{
private:
	ISystemMesh* systemMesh;
	ISystemSound* systemSound;
	ISystemPhysics* systemPhysics;
	ISystemAI* systemAI;
	ISystemScripting* systemScriptingLua;
	IFactory<IComponent>* componentFactory;
	IFactory<IComponent>* videoFactory;
	std::function<void(const ISystemDataPersistency::OnFileLoad_s&)> onFileLoadCallback;
	void GetFramebufferReferences(rapidxml::xml_node<char>* root, Scene* scene);
	void GetCamera(rapidxml::xml_node<char>* root, Scene* scene);
	void GetSounds(rapidxml::xml_node<char>* root, Scene* scene);
	void GetLights(rapidxml::xml_node<char>* root, Scene* scene);
	void GetEntities(rapidxml::xml_node<char>* root, Scene* scene);
	void GetAnimations(rapidxml::xml_node<char>* root, Scene* scene);
	void GetScenes(rapidxml::xml_node<char>* root, Level* level);
	void GetWorldProperties(rapidxml::xml_node<char>* root, Scene* scene);
	void GetFrameBuffers(rapidxml::xml_node<char>* root, Level* level);
	void GetAI(rapidxml::xml_node<char>* root, Scene* scene);

	Entity* CreateEntity(rapidxml::xml_node<char>* root, Scene* scene);

	Level* GetLevel(const std::string& levelPath);
	std::vector<Level*> GetLevels(const std::string& chapterDefinitionFile);
	bool GetLevels(const std::string& chapterDefinitionFile, std::vector<std::string>& paths);

public:
	LevelFactory();
	~LevelFactory();

	void BindOnFileLoadCallback(std::function<void(const ISystemDataPersistency::OnFileLoad_s&)> fn);
	Level* Create(ParameterContainer& parameters);	
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
	void Dispose();
};