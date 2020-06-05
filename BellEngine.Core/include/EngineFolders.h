/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <string>

#define FOLDER_ASSETS			"assets"
#define FOLDER_SHADERS			"shaders"
#define FOLDER_MODELS			"models"
#define FOLDER_SOUNDS			"sounds"
#define FOLDER_TEXTURES			"textures"
#define FOLDER_CONFIG			"config"
#define FOLDER_FONTS			"fonts"
#define FOLDER_SAVE				"save"
#define FOLDER_SAVEDSTATES		"savedstates"
#define FOLDER_SKYBOXES			"skyboxes"
#define FOLDER_SCRIPTS			"scripts"

#define FOLDER_SAVE_SAVEDSTATES	"\\" + FOLDER_SAVE + "\\" + FOLDER_SAVEDSTATES
#define FOLDER_ASSETS_SHADERS	"\\" + FOLDER_ASSETS + "\\" + FOLDER_SHADERS
#define FOLDER_ASSETS_SOUNDS	"\\" + FOLDER_ASSETS + "\\" + FOLDER_SOUNDS
#define FOLDER_ASSETS_MODELS	"\\" + FOLDER_ASSETS + "\\" + FOLDER_MODELS
#define FOLDER_ASSETS_TEXTURES	"\\" + FOLDER_ASSETS + "\\" + FOLDER_TEXTURES
#define FOLDER_ASSETS_CONFIG	"\\" + FOLDER_ASSETS + "\\" + FOLDER_CONFIG
#define FOLDER_ASSETS_FONTS		"\\" + FOLDER_ASSETS + "\\" + FOLDER_FONTS
#define FOLDER_ASSETS_SKYBOXES	"\\" + FOLDER_ASSETS + "\\" + FOLDER_SKYBOXES
#define FOLDER_ASSETS_SCRIPTS	"\\" + FOLDER_ASSETS + "\\" + FOLDER_SCRIPTS

class EngineFolders
{
public:
	static std::string workingDirectory;

	EngineFolders();
	~EngineFolders();

	static std::string GetWorkingDirectory();
	static std::string GetAssetsPath();
	static std::string GetShadersPath();
	static std::string GetModelsPath();
	static std::string GetSoundsPath();
	static std::string GetFontsPath();
	static std::string GetConfigPath();
	static std::string GetSavePath();
	static std::string GetSaveStatePath();
	static std::string GetTexturesPath();
	static std::string GetSkyboxesPath();
	static std::string GetScriptsPath();
};