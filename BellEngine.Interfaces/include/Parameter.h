#pragma once
#include <string>

struct Parameter
{
	bool variable;
	std::string type;
	std::string name;
	std::string value;
	void* valuePtr;
	Parameter() : variable(false), valuePtr(nullptr) {}
	Parameter(std::string type, std::string name, void* value, bool variable)
	{
		this->type = type;
		this->name = name;
		this->valuePtr = value;
		this->variable = variable;
	}
	Parameter(std::string type, std::string name, std::string value, bool variable)
	{
		this->type = type;
		this->name = name;
		this->value = value;
		this->variable = variable;
	}
};