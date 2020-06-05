/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include "EngineFolders.h"
#if __cplusplus >= 201703L
	#include <filesystem>
#else
	#include <direct.h>
#endif

std::string EngineFolders::workingDirectory = EngineFolders::GetWorkingDirectory();

EngineFolders::EngineFolders()
{

}

EngineFolders::~EngineFolders()
{

}

std::string EngineFolders::GetWorkingDirectory()
{
	std::string pathStr;

#if __cplusplus >= 201703L
	pathStr = std::filesystem::current_path().string();
#else
	char path[FILENAME_MAX];
	if (_getcwd(path, sizeof(path)))
	{
		pathStr = path;
	}
#endif

	return pathStr;
}

std::string EngineFolders::GetAssetsPath()
{
	return workingDirectory + FOLDER_ASSETS + "\\";
}

std::string EngineFolders::GetShadersPath()
{
	return workingDirectory + FOLDER_ASSETS_SHADERS + "\\";
}

std::string EngineFolders::GetModelsPath()
{
	return workingDirectory + FOLDER_ASSETS_MODELS + "\\";
}

std::string EngineFolders::GetSoundsPath()
{
	return workingDirectory + FOLDER_ASSETS_SOUNDS + "\\";
}

std::string EngineFolders::GetFontsPath()
{
	return workingDirectory + FOLDER_ASSETS_FONTS + "\\";
}

std::string EngineFolders::GetConfigPath()
{
	return workingDirectory + FOLDER_ASSETS_CONFIG + "\\";
}

std::string EngineFolders::GetSavePath()
{
	return workingDirectory + FOLDER_SAVE + "\\";
}

std::string EngineFolders::GetSaveStatePath()
{
	return workingDirectory + FOLDER_SAVE_SAVEDSTATES; +"\\";
}

std::string EngineFolders::GetTexturesPath()
{
	return workingDirectory + FOLDER_ASSETS_TEXTURES + "\\";
}

std::string EngineFolders::GetSkyboxesPath()
{
	return workingDirectory + FOLDER_ASSETS_SKYBOXES + "\\";
}

std::string EngineFolders::GetScriptsPath()
{
	return workingDirectory + FOLDER_ASSETS_SCRIPTS + "\\";
}
