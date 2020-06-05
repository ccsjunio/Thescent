#include "LuaScript.h"
#include <StringHelpers.h>
#include <BellEngineID.h>

void LuaScript::ClearParameters()
{
	parameters.Clear();
}

LuaScript::LuaScript()
{
}

LuaScript::~LuaScript()
{
	ClearParameters();
}

void LuaScript::SetScript(const std::string& path)
{
	this->path = path;

	if (!this->path.empty())
	{
		std::string name = this->path.substr(this->path.find_last_of("/\\") + 1);
		this->name = name + "_" + StringHelpers::GenerateRandomString(16);
		this->parameters.Add("std::string", "UNIQUE_SCRIPT_NAME", this->name);
		this->rawContent = StringHelpers::GetAllLinesFromFile(this->path);
	}
}

const std::string LuaScript::GetPath()
{
	return path;
}

/*Returns a "pre-compiled" version of the script, where the parameters are replaced with the user input values.*/
std::string LuaScript::GetContent()
{
	std::string content = rawContent;

	std::vector<Parameter> params;
	if (parameters.GetParameters(params))
	{
		char buf[255];
		for (Parameter element : params)
		{
			if (element.type == "std::string" || element.type == "string")
			{
				sprintf_s(buf, 255, "\"%s\"", element.value.c_str());
			}
			else
			{
				sprintf_s(buf, 255, "%s", element.value.c_str());
			}

			content = StringHelpers::StringReplace(content.c_str(), element.name.c_str(), buf);
		}
	}

	return content;
}

std::string LuaScript::GetRawContent()
{
	return rawContent;
}

const bool LuaScript::SetParameters(const ParameterContainer& parameters)
{
	this->parameters = parameters;
	return this->parameters.GetCount() > 0;
}

const bool LuaScript::GetParameters(ParameterContainer& parameters)
{
	parameters = this->parameters;
	return parameters.GetCount() > 0;
}

const bool LuaScript::AddParameters(ParameterContainer& parameters)
{
	std::vector<Parameter> params;
	if (parameters.GetParameters(params))
	{
		for (Parameter p : params)
		{
			this->parameters.Add(p);
		}

		return true;
	}

	return false;
}

const unsigned long long LuaScript::GetTypeID()
{
	return ENGINE_OBJ_ID_SCP_LUASCRIPT;
}

const std::string LuaScript::GetType()
{
	return "LuaScript";
}

const std::string LuaScript::GetName()
{
	return this->name;
}

void LuaScript::SetName(const std::string& name)
{
	this->name = name;
}
