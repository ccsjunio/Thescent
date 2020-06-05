#pragma once
#include <IScript.h>
#include <map>
#include <ParameterContainer.h>

class LuaScript : public IScript
{
private:
	std::string name;
	std::string path;
	std::string rawContent;
	ParameterContainer parameters;

public:
	LuaScript();
	~LuaScript();

	void SetScript(const std::string& path);
	void ClearParameters();

	const std::string GetPath();
	/*Returns a "pre-compiled" version of the script, where the parameters are replaced with the user input values.*/
	std::string GetContent();
	/*Returns the actual script as read from the path.*/
	std::string GetRawContent();

	const bool SetParameters(const ParameterContainer& parameters);
	const bool GetParameters(ParameterContainer& parameters);
	const bool AddParameters(ParameterContainer& parameters);
	const unsigned long long GetTypeID();
	const std::string GetType();
	const std::string GetName();
	void SetName(const std::string& name);
};

