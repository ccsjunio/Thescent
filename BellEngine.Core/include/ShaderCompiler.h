/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include "Shader.h"

class ShaderCompiler
{
private:
	

public:
	std::string m_lastError;
	bool IsShaderCompiled(unsigned int shaderID, std::string& errorText);
	bool IsLinkErrorFound(unsigned int programID, std::string& errorText);
};

