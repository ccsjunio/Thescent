#include "ShaderProgramFactory.h"
#include <ShaderCompiler.h>
#include "ShaderProgram.h"
#include <GLCommon.h>

IShaderProgram* ShaderProgramFactory::CreateShaderProgram(ParameterContainer& parameters)
{
	Parameter programName;
	bool rc = parameters.FindParameterByName("Name", programName);
	if (!rc) return nullptr;
	if (programName.value.empty()) return nullptr;

	Parameter shadersParameter;
	rc = parameters.FindParameterByName("Shaders", shadersParameter);
	if (!rc) return nullptr;

	if (!shadersParameter.valuePtr) return nullptr;

	std::vector<Shader*> shaders = *(std::vector<Shader*>*)shadersParameter.valuePtr;
	   
	ShaderCompiler* shaderCompiler = new ShaderCompiler(); //For debugging purposes
	ShaderProgram* _shaderProgram = new ShaderProgram();
	_shaderProgram->SetName(programName.value);

	GLuint program = glCreateProgram();;

	unsigned int handlerID = 0;
	std::string shaderContent;
	for (auto& shader : shaders)
	{
		handlerID = 0;
		
		std::string shaderType = shader->GetType();
		if (shaderType == "VERTEX_SHADER")
		{
			handlerID = glCreateShader(GL_VERTEX_SHADER);
		}
		else if (shaderType == "FRAGMENT_SHADER")
		{
			handlerID = glCreateShader(GL_FRAGMENT_SHADER);
		}
		else if (shaderType == "GEOMETRY_SHADER")
		{
			handlerID = glCreateShader(GL_GEOMETRY_SHADER);
		}
		else
		{
			continue;
		}

		shaderContent = shader->GetContent();
		const char* shaderCode = shaderContent.c_str();
		glShaderSource(handlerID, 1, &shaderCode, NULL);
		glCompileShader(handlerID);

		//Checks if something went wrong during shader compilation
		std::string errStr;
		if (shaderCompiler->IsShaderCompiled(handlerID, errStr))
		{
			glAttachShader(program, handlerID);
			shader->SetHandlerID(handlerID);
		}
		else
		{

		}
	}

	glLinkProgram(program);

	//Checks if something went wrong during shader compilation
	std::string errStr;
	bool err = shaderCompiler->IsLinkErrorFound(program, errStr);
	if (err)
	{
		//Handle error
	}

	_shaderProgram->SetID(program);

	//ShaderPrograms.push_back(_shaderProgram);

	if (shaderCompiler)
	{
		delete shaderCompiler;
	}

	return _shaderProgram;
}

ShaderProgramFactory::ShaderProgramFactory()
{
}

ShaderProgramFactory::~ShaderProgramFactory()
{
}

IShaderProgram* ShaderProgramFactory::Create(ParameterContainer& parameters)
{
	Parameter typeParam;
	bool rc = parameters.FindParameterByName("Type", typeParam);
	if (!rc) return nullptr;

	IShaderProgram* ptr = nullptr;

	if (typeParam.value == "ShaderProgram")
	{
		ptr = CreateShaderProgram(parameters);
	}

	return ptr;
}

const bool ShaderProgramFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return true;
}
