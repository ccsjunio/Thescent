#pragma once
#include <vector>
#include <GLMCommon.h>
#include <GLCommon.h>
#include <IGLVertexShader.h>

class BasicVertexShader : public IGLVertexShader
{
private:
	std::string name;

	unsigned int programID;
	
	GLint renderMode;
	ModelDrawInfo mdi;

	GLint modelMatrix_ul, enableOffsetAlongNormal_ul, vertexOffset_ul;

	const bool GetUniformLocations();

public:
	BasicVertexShader();
	~BasicVertexShader();

	const unsigned long long GetTypeID();
	const std::string GetType();
	const std::string GetName();
	void SetName(const std::string& name);

	const bool Initialize();
	void SetProgramID(const unsigned int& programID);
	void SetRenderMode(const int& value);
	void SetDrawInfo(const ModelDrawInfo& mdi);
	void Draw();

	const bool SetModelMatrix(const Matrix4D& value);
	const bool SetEnableOffsetAlongNormal(const bool& value);
	const bool SetVertexOffset(const float& value);
};

