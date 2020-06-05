#include "Script.h"
#include <StringHelpers.h>

void Script::ClearParameters()
{	
	parameters.clear();
}

Script::Script()
{
}

Script::~Script()
{
	ClearParameters();
}

void Script::SetScript(std::string path)
{	
	this->path = path;

	if (!this->path.empty())
	{
		std::string name = this->path.substr(this->path.find_last_of("/\\") + 1);
		this->name = name + "_" + StringHelpers::GenerateRandomString(16);
		this->parameters["UNIQUE_SCRIPT_NAME"] = Parameter("string", "UNIQUE_SCRIPT_NAME", this->name, false);
		this->rawContent = StringHelpers::GetAllLinesFromFile(this->path);
	}
}

std::string Script::GetName()
{
	return name;
}

std::string Script::GetPath()
{
	return path;
}

/*Returns a "pre-compiled" version of the script, where the parameters are replaced with the user input values.*/
std::string Script::GetContent()
{
	std::string content = rawContent;

	char buf[255];
	for (std::pair<std::string, Parameter> element : parameters)
	{
		if (element.second.type == "string")
		{
			sprintf_s(buf, 255, "\"%s\"", element.second.value.c_str());
		}
		else
		{
			sprintf_s(buf, 255, "%s", element.second.value.c_str());
		}

		content = StringHelpers::StringReplace(content.c_str(), element.first.c_str(), buf);
	}

	return content;
}

std::string Script::GetRawContent()
{
	return rawContent;
}
