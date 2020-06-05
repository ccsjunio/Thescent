/***********************************************************************
* Project           : Game
* Author            : Felipe Bellini
* Date created      : Oct. 12 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include "KeyBindings.h"

/*This class was changed so users can swap the physics engine using the config file.*/

class GameConfig
{
private:
	void GetDataPersistencyOptions(const std::string& cfgFileContent);
	void GetAIOptions(const std::string& cfgFileContent);
	void GetMeshOptions(const std::string& cfgFileContent);
	void GetSoundOptions(const std::string& cfgFileContent);
	void GetPhysicsOptions(const std::string& cfgFileContent);
	void GetControlOptions(const std::string& cfgFileContent);
	void GetVideoConfig(const std::string& cfgFileContent);
	void GetKeyBindings(const std::string& cfgFileContent);
	void GetLuaScriptingConfig(const std::string& cfgFileContent);

public:
	struct GUIOptions_e
	{
	public:
		unsigned int engine;
		bool isActive;
		GUIOptions_e() : engine(0), isActive(true) {};
	};
	struct ControlOptions_s
	{
	public:
		unsigned int engine;
		bool isActive;
		float mouseSensitivity, movementStep;
		ControlOptions_s() : mouseSensitivity(0.0f), movementStep(0.0f), engine(0), isActive(true) {};
	};
	struct MeshOptions_s
	{
	public:
		unsigned int engine;
		bool isActive;
		MeshOptions_s() : engine(0), isActive(true) {};
	};
	struct SoundOptions_s
	{
	public:
		unsigned int engine;
		bool isActive;
		SoundOptions_s() : engine(0), isActive(true) {};
	};
	struct PhysicsOptions_s
	{
	public:
		unsigned int engine;
		bool isActive;
		PhysicsOptions_s() : engine(0), isActive(true) {};
	};
	struct AIOptions_s
	{
	public:
		unsigned int engine;
		bool isActive;
		AIOptions_s() : engine(0), isActive(true) {};
	};
	struct VideoOptions_s
	{
	public:
		unsigned int WindowWidth;
		unsigned int WindowHeight;
		bool isActive;
		VideoOptions_s() : WindowWidth(640), WindowHeight(480), isActive(true) {};
	};
	struct DataPersistencyOptions_s
	{
		unsigned int engine;
		bool isActive;
		DataPersistencyOptions_s() : engine(0), isActive(true) {};
	};
	struct LuaScriptingOptions_s
	{
	public:
		bool isActive;
		LuaScriptingOptions_s() : isActive(true) {};
	};

	//Video settings
	KeyBindings* keyBindings;
	VideoOptions_s videoOptions;	
	AIOptions_s aiOptions;
	SoundOptions_s soundOptions;
	MeshOptions_s meshOptions;
	ControlOptions_s controlOptions;
	PhysicsOptions_s physicsOptions;
	DataPersistencyOptions_s dataPersistencyOptions;
	GUIOptions_e guiOptions;
	LuaScriptingOptions_s luaScriptingOptions;

	GameConfig() :
		controlOptions(), 
		videoOptions(), 
		keyBindings(nullptr),
		aiOptions(),
		soundOptions(), 
		meshOptions(),
		physicsOptions(),
		dataPersistencyOptions(),
		luaScriptingOptions(),
		guiOptions()
	{};
	GameConfig(const std::string& cfgPath);
	~GameConfig();

	void LoadCFGContent(const std::string& cfgFileContent);
};

