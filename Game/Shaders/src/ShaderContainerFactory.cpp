#include "../include/ShaderContainerFactory.h"
#include <GLCommon.h>
#include <ShaderProgram.h>
#include "../include/GLShaderContainer.h"
#include <StringHelpers.h>
#include <EngineFolders.h>

IShaderContainer* ShaderContainerFactory::CreateGLShaderContainer(ParameterContainer& parameters)
{
	Parameter p;
	if (!parameters.FindParameterByName("parameters_collection", p)) return nullptr;
	std::vector<ParameterContainer>* parameters_collection = (std::vector<ParameterContainer>*)p.valuePtr;

	if (!parameters_collection->empty())
	{
		GLShaderContainer* container = new GLShaderContainer();

		std::vector<ShaderProgramDef> infos;

		for (auto& pc : *parameters_collection)
		{			
			ShaderProgramDef spi;

			Parameter name;
			pc.FindParameterByName("Name", name);
			spi.name = name.value;

			Parameter shaderCount;
			pc.FindParameterByName("ShaderCount", shaderCount);
			unsigned long long shaderCount_ul = strtoull(shaderCount.value.c_str(), NULL, 0);

			for (size_t i = 0; i < shaderCount_ul; i++)
			{
				bool path_found = false, type_found = false;
				std::string path;
				unsigned long long type;
				Parameter _p;

				if (pc.FindParameterByName("Shader" + std::to_string(i) + "Path", _p))
				{
					path = EngineFolders::GetShadersPath() + _p.value;
					path_found = true;
				}

				if (pc.FindParameterByName("Shader" + std::to_string(i) + "Type", _p))
				{
					type = strtoull(_p.value.c_str(), NULL, 0);
					type_found = true;
				}

				if (path_found && type_found)
				{
					spi.shaders.push_back(std::pair<unsigned long long, std::string>(type, path));
				}
			}

			Parameter availableFeautures;
			if (pc.FindParameterByName("AvailableFeatures", availableFeautures))
			{
				std::stringstream ss(availableFeautures.value.c_str());
				std::string temp;
				while (ss >> temp)
				{
					spi.availableShaderFeatures.push_back(temp);
				}
			}

			Parameter availableFBOPasses;
			if (pc.FindParameterByName("AvailableFBOPasses", availableFBOPasses))
			{
				std::stringstream ss(availableFBOPasses.value.c_str());
				std::string temp;
				while (ss >> temp)
				{
					spi.availableFBOPasses.push_back(temp);
				}
			}

			infos.push_back(spi);			
		}

		for (auto& spi : infos)
		{
			ParameterContainer params;
			params.Add("ShaderProgramInfo", "spi", &spi);
			params.Add("GLShaderContainer", "Container", container);

			IShaderProgram* program = CreateShaderProgram(params);
			if (program)
			{
				container->AddShaderProgram(program);
			}
		}

		delete parameters_collection;
		parameters_collection = nullptr;

		return container;
	}


	return nullptr;
}

IShader* ShaderContainerFactory::CreateShader(ParameterContainer& parameters)
{
	Parameter p;

	std::string path;
	if (!parameters.FindParameterByName("ShaderPath", p)) return nullptr;
	path = p.value;

	unsigned long long type = 0;
	if (!parameters.FindParameterByName("ShaderType", p)) return nullptr;
	type = *(unsigned long long*)p.valuePtr;

	std::string filename = StringHelpers::GetFileNameFromFilePath(path);
	std::string name = StringHelpers::GetNameWithoutExtension(filename);

	//Create Shader
	Shader* shader = new Shader();
	shader->SetPath(path);
	shader->SetName(name);
	shader->SetContent(StringHelpers::GetAllLinesFromFile(path));
	shader->SetTypeID(type);

	//Scan shader, looking for type, version, etc.
	std::stringstream ss(shader->GetContent().c_str());
	std::string temp;

	while (ss >> temp)
	{
		if (temp == "#version")
		{
			unsigned int version = 0;
			ss >> version;
			shader->SetVersion(version);
			break;
		}
	}

	return shader;
}

IShaderProgram* ShaderContainerFactory::CreateShaderProgram(ParameterContainer& parameters)
{
	Parameter p;

	ShaderProgramDef shaderData;
	if (!parameters.FindParameterByName("spi", p)) return nullptr;
	shaderData = *(ShaderProgramDef*)p.valuePtr;

	GLShaderContainer* container = nullptr;
	if (!parameters.FindParameterByName("Container", p)) return nullptr;
	container = (GLShaderContainer*)p.valuePtr;
	if (!container) return nullptr;
		
	std::vector<IShader*> shaders;
	for (auto& data : shaderData.shaders)
	{
		ParameterContainer _pc;
		_pc.Add("unisgned long long", "ShaderType", &data.first);
		_pc.Add("std::string", "ShaderPath", data.second);

		IShader* shader = CreateShader(_pc);
		if (shader)
		{
			shaders.push_back(shader);
		}
	}

	ShaderProgram* _shaderProgram = new ShaderProgram();
	_shaderProgram->SetName(shaderData.name);
	
	for (auto& element : shaderData.availableShaderFeatures)
	{
		_shaderProgram->AddFeatureSupport(element);
	}

	for (auto& element : shaderData.availableFBOPasses)
	{
		_shaderProgram->AddFBOPass(element);
	}

	GLuint program = glCreateProgram();;

	unsigned int handlerID = 0;
	std::string shaderContent;
	if (!shaders.empty())
	{
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
				_shaderProgram->AddShader(shader);
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
		if (shaderCompiler->IsLinkErrorFound(program, errStr))
		{
			//Handle error
		}

		_shaderProgram->SetID(program);

		return _shaderProgram;
	}

	if (_shaderProgram)
	{
		delete (_shaderProgram);
		_shaderProgram = nullptr;
	}

	return nullptr;
}

ShaderContainerFactory::ShaderContainerFactory()
{
	this->shaderCompiler = new ShaderCompiler();
}

ShaderContainerFactory::~ShaderContainerFactory()
{
	if (this->shaderCompiler)
	{
		delete (this->shaderCompiler);
		this->shaderCompiler = nullptr;
	}
}

IShaderContainer* ShaderContainerFactory::Create(ParameterContainer& parameters)
{
	Parameter p;
	if (parameters.FindParameterByName("Type", p))
	{
		if (p.value == "GLShaderContainer")
		{
			return CreateGLShaderContainer(parameters);
		}
	}

	return nullptr;
}

const bool ShaderContainerFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return false;
}
