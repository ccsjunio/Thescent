#pragma once
#include <IFactory.h>
#include <IScript.h>

class LuaScriptFactory : public IFactory<IScript>
{
private:
	IScript* CreateLuaScript(ParameterContainer& parameters);

public:
	LuaScriptFactory();
	~LuaScriptFactory();

	IScript* Create(ParameterContainer& parameters);

	const bool Initialize(ParameterContainer& input, ParameterContainer& output);

};

