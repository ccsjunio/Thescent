/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#include "Shader.h"
#include <BellEngineID.h>

Shader::Shader()
{
	versionSupport = 0;
	id = 0;
	type = Shader::eShaderType::UNKNOWN;
	bSourceIsMultiLine = false;
}

Shader::~Shader()
{
}

void Shader::SetVersion(const unsigned int& value)
{
	this->versionSupport = value;
}

const unsigned int Shader::GetVersion()
{
	return this->versionSupport;
}

void Shader::SetTypeID(const unsigned long long& typeID)
{
	this->type = (Shader::eShaderType)typeID;
}

void Shader::SetPath(const std::string& value)
{
	this->path = value;
}

const std::string Shader::GetPath()
{
	return this->path;
}

void Shader::SetContent(const std::string& value)
{
	this->content = value;
}

const std::string Shader::GetContent()
{
	return this->content;
}

const unsigned long long Shader::GetTypeID()
{
	return (unsigned long long)this->type;
}

const unsigned int Shader::GetHandlerID()
{
	return this->id;
}

void Shader::SetHandlerID(const unsigned int& value)
{
	this->id = value;
}

const std::string Shader::GetName()
{
	return name;
}

void Shader::SetName(const std::string& name)
{
	this->name = name;
}

const std::string Shader::GetType()
{
	std::string typeStr;

	switch (type)
	{
	case Shader::eShaderType::VERTEX_SHADER: typeStr = "VERTEX_SHADER"; break;
	case Shader::eShaderType::FRAGMENT_SHADER: typeStr = "FRAGMENT_SHADER"; break;
	case Shader::eShaderType::GEOMETRY_SHADER: typeStr = "GEOMETRY_SHADER"; break;
	case Shader::eShaderType::UNKNOWN:
	default:
		typeStr = "UNKNOWN"; break;
	}

	return typeStr;
}
