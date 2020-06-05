/***********************************************************************
* Project           : Game
* Author            : Felipe Bellini
* Date created      : Oct. 12 - 2019
* Licensed under:	: MIT

************************************************************************/
#include "GameConfig.h"
#include "KeyBindings.h"
#include <sstream>
#include <StringHelpers.h>
#include "GameConfigKeywords.h"

GameConfig::GameConfig(const std::string& cfgPath)
{
	keyBindings = new KeyBindings();

	std::string cfgFileContent = StringHelpers::GetAllLinesFromFile(cfgPath);
	LoadCFGContent(cfgFileContent);
}

GameConfig::~GameConfig()
{
	if (keyBindings)
	{
		delete keyBindings;
		keyBindings = nullptr;
	}
}

void GameConfig::LoadCFGContent(const std::string& cfgFileContent)
{
	GetVideoConfig(cfgFileContent);
	GetKeyBindings(cfgFileContent);
	GetControlOptions(cfgFileContent);
	GetPhysicsOptions(cfgFileContent);
	GetSoundOptions(cfgFileContent);
	GetMeshOptions(cfgFileContent);
	GetAIOptions(cfgFileContent);
	GetDataPersistencyOptions(cfgFileContent);
	GetLuaScriptingConfig(cfgFileContent);
}

void GameConfig::GetDataPersistencyOptions(const std::string& cfgFileContent)
{
	std::stringstream ss(cfgFileContent.c_str());

	// Scan to vertex
	std::string temp;
	bool getProperties = false;
	while (ss >> temp)
	{
		//Video Properties
		if (temp == CFG_KEYWORD_DATAPERSISTENCY)
		{
			getProperties = !getProperties;
		}

		if (getProperties)
		{
			if (temp == CFG_PROPERTY_ISACTIVE)		ss >> dataPersistencyOptions.isActive;
			std::string t;
			if (temp == CFG_PROPERTY_ENGINE)
			{
				ss >> t;
				dataPersistencyOptions.engine = atoi(t.c_str());
			}
		}

		if (temp == CFG_KEYWORD_ENDL_DATAPERSISTENCY)
		{
			getProperties = !getProperties;
			break;
		}
	}
}

void GameConfig::GetAIOptions(const std::string& cfgFileContent)
{
	std::stringstream ss(cfgFileContent.c_str());

	// Scan to vertex
	std::string temp;
	bool getProperties = false;
	while (ss >> temp)
	{
		//Video Properties
		if (temp == CFG_KEYWORD_AI)
		{
			getProperties = !getProperties;
		}

		if (getProperties)
		{
			if (temp == CFG_PROPERTY_ISACTIVE)		ss >> aiOptions.isActive;
			std::string t;
			if (temp == CFG_PROPERTY_ENGINE)
			{
				ss >> t;
				aiOptions.engine = atoi(t.c_str());
			}
		}

		if (temp == CFG_KEYWORD_ENDL_AI)
		{
			getProperties = !getProperties;
			break;
		}
	}
}

void GameConfig::GetMeshOptions(const std::string& cfgFileContent)
{
	std::stringstream ss(cfgFileContent.c_str());

	// Scan to vertex
	std::string temp;
	bool getProperties = false;
	while (ss >> temp)
	{
		//Video Properties
		if (temp == CFG_KEYWORD_MESH)
		{
			getProperties = !getProperties;
		}

		if (getProperties)
		{
			if (temp == CFG_PROPERTY_ISACTIVE)		ss >> meshOptions.isActive;
			std::string t;
			if (temp == CFG_PROPERTY_ENGINE)
			{
				ss >> t;
				meshOptions.engine = atoi(t.c_str());
			}
		}

		if (temp == CFG_KEYWORD_ENDL_MESH)
		{
			getProperties = !getProperties;
			break;
		}
	}
}

void GameConfig::GetSoundOptions(const std::string& cfgFileContent)
{
	std::stringstream ss(cfgFileContent.c_str());

	// Scan to vertex
	std::string temp;
	bool getProperties = false;
	while (ss >> temp)
	{
		//Video Properties
		if (temp == CFG_KEYWORD_SOUND)
		{
			getProperties = !getProperties;
		}

		if (getProperties)
		{
			if (temp == CFG_PROPERTY_ISACTIVE)		ss >> soundOptions.isActive;
			std::string t;
			if (temp == CFG_PROPERTY_ENGINE)
			{
				ss >> t;
				soundOptions.engine = atoi(t.c_str());
			}
		}

		if (temp == CFG_KEYWORD_ENDL_SOUND)
		{
			getProperties = !getProperties;
			break;
		}
	}
}

void GameConfig::GetPhysicsOptions(const std::string& cfgFileContent)
{
	std::stringstream ss(cfgFileContent.c_str());

	// Scan to vertex
	std::string temp;
	bool getProperties = false;
	while (ss >> temp)
	{
		//Video Properties
		if (temp == CFG_KEYWORD_PHYSICS)
		{
			getProperties = !getProperties;
		}

		if (getProperties)
		{
			if (temp == CFG_PROPERTY_ISACTIVE)		ss >> physicsOptions.isActive;
			if (temp == CFG_PROPERTY_ENGINE)		ss >> physicsOptions.engine;
		}

		if (temp == CFG_KEYWORD_ENDL_PHYSICS)
		{
			getProperties = !getProperties;
			break;
		}
	}
}

void GameConfig::GetControlOptions(const std::string& cfgFileContent)
{
	std::stringstream ss(cfgFileContent.c_str());

	// Scan to vertex
	std::string temp;
	bool getProperties = false;
	while (ss >> temp)
	{
		//Video Properties
		if (temp == CFG_KEYWORD_CONTROLOPTIONS)
		{
			getProperties = !getProperties;
		}

		if (getProperties)
		{
			if (temp == CFG_PROPERTY_ISACTIVE)		ss >> controlOptions.isActive;
			std::string t;
			if (temp == CFG_PROPERTY_MOUSESENSITIVITY)
			{
				ss >> t;
				controlOptions.mouseSensitivity = atof(t.c_str());
			}
			if (temp == CFG_PROPERTY_MOVEMENTSTEP)
			{
				ss >> t;
				controlOptions.movementStep = atof(t.c_str());
			}
		}

		if (temp == CFG_KEYWORD_ENDL_CONTROLOPTIONS)
		{
			getProperties = !getProperties;
			break;
		}
	}
}

void GameConfig::GetVideoConfig(const std::string& cfgFileContent)
{
	std::stringstream ss(cfgFileContent.c_str());

	// Scan to vertex
	std::string temp;
	bool getProperties = false;
	while (ss >> temp)
	{
		//Video Properties
		if (temp == CFG_KEYWORD_VIDEO)
		{
			getProperties = !getProperties;
		}

		if (getProperties)
		{
			if (temp == CFG_PROPERTY_ISACTIVE)		ss >> videoOptions.isActive;
			if (temp == CFG_PROPERTY_WINDOWHEIGHT)	ss >> videoOptions.WindowHeight;
			if (temp == CFG_PROPERTY_WINDOWWIDTH)	ss >> videoOptions.WindowWidth;
		}

		if (temp == CFG_KEYWORD_ENDL_VIDEO)
		{
			getProperties = !getProperties;
			break;
		}
	}
}

void GameConfig::GetKeyBindings(const std::string& cfgFileContent)
{
	std::stringstream ss(cfgFileContent.c_str());

	// Scan to vertex
	std::string temp;
	bool getProperties = false;

	while (ss >> temp)
	{
		if (temp == CFG_KEYWORD_KEYBINDINGS)
		{
			getProperties = !getProperties;
			if (getProperties)
			{
				//Next word because keybinding has an id
				ss >> temp;
				continue;
			}
			else
			{
				break;
			}
		}

		int tempVal;
		if (getProperties)
		{
			ss >> tempVal;
			keyBindings->RegisterKeyBind(temp, tempVal);
		}

		if (temp == CFG_KEYWORD_ENDL_KEYBINDINGS)
		{
			getProperties = !getProperties;
			break;
		}
	}

	keyBindings->LoadKeys();
}

void GameConfig::GetLuaScriptingConfig(const std::string& cfgFileContent)
{
	std::stringstream ss(cfgFileContent.c_str());

	// Scan to vertex
	std::string temp;
	bool getProperties = false;
	while (ss >> temp)
	{
		//Video Properties
		if (temp == CFG_KEYWORD_LUASCRIPTING)
		{
			getProperties = !getProperties;
		}

		if (getProperties)
		{
			if (temp == CFG_PROPERTY_ISACTIVE)		ss >> luaScriptingOptions.isActive;
		}

		if (temp == CFG_KEYWORD_ENDL_LUASCRIPTING)
		{
			getProperties = !getProperties;
			break;
		}
	}
}
