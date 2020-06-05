/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <IShaderProgram.h>
#include <IDisposable.h>
#include <vector>

class ShaderProgram : public IShaderProgram, public IDisposable
{
private:
	unsigned int id;
	std::string name;
	std::vector<std::string> featureSupport, fboPasses;
	std::vector<IShader*> shaders;

public:
	ShaderProgram();
	~ShaderProgram();

	const bool AddShader(IShader*& value);
	const bool RemoveShader(IShader*& value, const bool& deletePtr = true);
	const bool GetShaders(std::vector<IShader*>& value);
	IShader* FindShaderByName(const std::string& value);

	const bool AddFBOPass(const std::string& value);
	const bool RemoveFBOPass(const std::string& value);
	const bool HasFBOPass(const std::string& value);
	const bool AddFeatureSupport(const std::string& value);
	const bool RemoveFeatureSupport(const std::string& value);
	const bool HasFeatureSupport(const std::string& value);
	const std::string GetName();
	void SetName(const std::string& name);
	const unsigned long long GetTypeID();
	void SetID(unsigned int& id);
	unsigned int GetID();
	const std::string GetType();
	void Dispose();
};

