#pragma once
#include "IIdentifiable.h"
#include <string>
#include "IShader.h"
#include <vector>

class IShaderProgram : public IIdentifiable
{
protected:
	IShaderProgram() {};

public:
	virtual ~IShaderProgram() {};

	virtual const bool AddShader(IShader*& value) = 0;
	virtual const bool RemoveShader(IShader*& value, const bool& deletePtr = true) = 0;
	virtual const bool GetShaders(std::vector<IShader*>& value) = 0;
	virtual IShader* FindShaderByName(const std::string& value) = 0;

	virtual const bool AddFBOPass(const std::string& value) = 0;
	virtual const bool RemoveFBOPass(const std::string& value) = 0;
	virtual const bool HasFBOPass(const std::string& value) = 0;

	virtual const bool AddFeatureSupport(const std::string& value) = 0;
	virtual const bool RemoveFeatureSupport(const std::string& value) = 0;
	virtual const bool HasFeatureSupport(const std::string& value) = 0;
	virtual void SetID(unsigned int& id) = 0;
	virtual unsigned int GetID() = 0;
};