#pragma once
#include <Parameter.h>
#include <vector>
#include <string>

/*Custom costainer for keeping generic data, using messages and tc through the engine.*/
class ParameterContainer
{
protected:
	std::vector<Parameter> parameters;

public:
	ParameterContainer() {};
	~ParameterContainer() {};

	/*Inserts a parameter into the container*/
	const bool Add(const std::string& type, const std::string& name, void* value, const bool& variable = false)
	{
		return Add(Parameter(type, name, value, variable));
	}
	/*Inserts a parameter into the container*/
	const bool Add(const std::string& type, const std::string& name, const std::string& value, const bool& variable = false)
	{
		return Add(Parameter(type, name, value, variable));
	}
	/*Inserts a parameter into the container*/
	const bool Add(const Parameter& parameter)
	{
		std::vector<Parameter>::iterator it = std::find_if(parameters.begin(), parameters.end(), [&parameter](const Parameter& p) { return p.name == parameter.name; });
		if (it == parameters.end())
		{
			parameters.push_back(parameter);
			return true;
		}

		return false;
	}
	/*Inserts a ParameterContainer into the container*/
	const bool Add(ParameterContainer& container)
	{
		size_t startSz = parameters.size();

		std::vector<Parameter> pars;
		if (container.GetParameters(pars))
		{
			for (auto& p : pars)
			{
				Add(p);
			}
		}

		return startSz < pars.size();
	}
	/*Clears all parameters from the container*/
	const void Clear()
	{
		parameters.clear();
		parameters.shrink_to_fit();
	}
	/*Finds a parameter by its name.*/
	const bool FindParameterByName(const std::string& name, Parameter& parameter)
	{
		std::vector<Parameter>::iterator it = std::find_if(parameters.begin(), parameters.end(), [&name](const Parameter& p) { return p.name == name; });

		if (it != parameters.end())
		{
			parameter = *it;
			return true;
		}

		parameter = Parameter();
		return false;
	}
	/*Finds parameter by its index.*/
	const bool FindParameterByIndex(const unsigned long& index, Parameter& parameter)
	{
		if (index < parameters.size())
		{
			for (unsigned long i = 0; i < parameters.size(); i++)
			{
				if (i == index)
				{
					parameter = parameters[i];
					break;
				}
			}

			return true;
		}

		return false;
	}
	/*Gets the number of parameters in the container.*/
	const size_t GetCount()
	{
		return parameters.size();
	}
	/*Gets all parameters in a vector.*/
	const bool GetParameters(std::vector<Parameter>& parameters)
	{
		parameters = this->parameters;
		return parameters.size() != 0;
	}
	/*Checks if a parameter exists in this container.*/
	const bool HasParameter(const std::string& name)
	{		
		return std::find_if(parameters.begin(), parameters.end(), [&name](const Parameter& p) { return p.name == name; }) != parameters.end();
	}
	/*Deletes a parameter from the container.*/
	const bool Remove(const Parameter& parameter)
	{
		std::vector<Parameter>::iterator it = std::find_if(parameters.begin(), parameters.end(), [&parameter](const Parameter& p) { return p.name == parameter.name; });
		if (it != parameters.end())
		{
			parameters.erase(it);
			return true;
		}

		return false;
	}
	/*Deletes a parameter from the container by its name.*/
	const bool Remove(const std::string& parameterName)
	{
		std::vector<Parameter>::iterator it = std::find_if(parameters.begin(), parameters.end(), [&parameterName](const Parameter& p) { return p.name == parameterName; });
		if (it != parameters.end())
		{
			parameters.erase(it);
			return true;
		}

		return false;
	}
	/*Sets the value of a parameter inside the container.*/
	const bool SetValue(const std::string& parameterName, const std::string& value)
	{
		std::vector<Parameter>::iterator it = std::find_if(parameters.begin(), parameters.end(), [&parameterName](const Parameter& p) { return p.name == parameterName; });

		if (it != parameters.end())
		{
			Parameter parameter = *it;
			parameter.value = value;

			parameters.erase(it);

			parameters.push_back(parameter);

			return true;
		}
		else
		{
			return false;
		}
	}
	/*Sets the valuePtr of a parameter inside the container.*/
	const bool SetValue(const std::string& parameterName, void* value)
	{
		std::vector<Parameter>::iterator it = std::find_if(parameters.begin(), parameters.end(), [&parameterName](const Parameter& p) { return p.name == parameterName; });

		if (it != parameters.end())
		{
			Parameter parameter = *it;
			parameter.valuePtr = value;

			parameters.erase(it);

			parameters.push_back(parameter);

			return true;
		}
		else
		{
			return false;
		}
	}
};