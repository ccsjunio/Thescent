#include "LuaScriptFactory.h"
#include "LuaScript.h"

IScript* LuaScriptFactory::CreateLuaScript(ParameterContainer& parameters)
{
	Parameter p;

	std::string path;
	if (!parameters.FindParameterByName("Path", p)) return nullptr;
	path = p.value;
	parameters.Remove(p.name);

	LuaScript* script = new LuaScript();
	script->SetScript(path);
	script->AddParameters(parameters);

	return script;
}

LuaScriptFactory::LuaScriptFactory()
{
}

LuaScriptFactory::~LuaScriptFactory()
{
}

IScript* LuaScriptFactory::Create(ParameterContainer& parameters)
{
	Parameter p;

	if (parameters.FindParameterByName("Type", p))
	{
		parameters.Remove(p.name);

		if (p.value == "LuaScript")
		{
			return CreateLuaScript(parameters);
		}
	}

	return nullptr;
}

const bool LuaScriptFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return false;
}
