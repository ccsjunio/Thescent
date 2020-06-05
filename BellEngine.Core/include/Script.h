#pragma once
#include <string>
#include <map>
#include <Parameter.h>

class Script
{
private:
	std::string name;
	std::string path;
	std::string rawContent;
	
public:
	Script();
	~Script();

	std::map<std::string, Parameter> parameters;
	
	void SetScript(std::string path);
	void ClearParameters();

	std::string GetName();
	std::string GetPath();
	/*Returns a "pre-compiled" version of the script, where the parameters are replaced with the user input values.*/
	std::string GetContent();
	/*Returns the actual script as read from the path.*/
	std::string GetRawContent();
};

