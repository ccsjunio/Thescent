#pragma once
#include "IIdentifiable.h"
#include <map>
#include <Parameter.h>
#include<ParameterContainer.h>

class IScript : public IIdentifiable
{
protected:
	IScript() {};

public:
	virtual ~IScript() {};

	virtual void SetScript(const std::string& path) = 0;
	virtual void ClearParameters() = 0;

	virtual const bool SetParameters(const ParameterContainer& parameters) = 0;
	virtual const bool GetParameters(ParameterContainer& parameters) = 0;
	virtual const bool AddParameters(ParameterContainer& parameters) = 0;
	virtual const std::string GetPath() = 0;
	/*Returns a "pre-compiled" version of the script, where the parameters are replaced with the user input values.*/
	virtual std::string GetContent() = 0;
	/*Returns the actual script as read from the path.*/
	virtual std::string GetRawContent() = 0;
};