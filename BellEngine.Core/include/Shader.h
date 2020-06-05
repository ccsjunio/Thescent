/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <string>
#include <vector>
#include "ProgrammingDef.h"
#include <IShader.h>

class Shader : public IShader
{
private:
	enum class eShaderType;
	Shader::eShaderType type;
	unsigned int id;	// or "name" from OpenGL
	std::string name;
	std::string path;
	std::string content;
	bool bSourceIsMultiLine;	
	unsigned int versionSupport;

	struct ShaderCodeField;
	std::vector<ShaderCodeField> Fields;

public:
	struct ShaderCodeField
	{
		std::string glVariableType;
	};

	enum class eShaderType
	{
		UNKNOWN,

		VERTEX_SHADER,
		FRAGMENT_SHADER,
		GEOMETRY_SHADER
	};

	Shader();
	~Shader();

	void SetVersion(const unsigned int& value);
	const unsigned int GetVersion();
	void SetTypeID(const unsigned long long& typeID);
	void SetPath(const std::string& value);
	const std::string GetPath();
	void SetContent(const std::string& value);
	const std::string GetContent();
	const unsigned int GetHandlerID();
	void SetHandlerID(const unsigned int& value);

	const unsigned long long GetTypeID();
	const std::string GetType();
	const std::string GetName();
	void SetName(const std::string& name);
};

