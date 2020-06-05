#pragma once
#include <vector>
#include <GLMCommon.h>
#include <GLCommon.h>
#include <IGLVertexShader.h>

class InstancedVertexShader : public IGLVertexShader
{
private:
	std::string name;

	unsigned int programID;
	size_t max_matrices_size;
	size_t current_matrices_size;
	GLint renderMode;
	ModelDrawInfo mdi;

	std::vector<GLint> matricesArray_ul;
	GLint matrices_ul, enableOffsetAlongNormal_ul, vertexOffset_ul;

	const bool GetUniformLocations();

public:
	InstancedVertexShader();
	~InstancedVertexShader();

	const unsigned long long GetTypeID();
	const std::string GetType();
	const std::string GetName();
	void SetName(const std::string& name);

	const bool Initialize();
	void SetProgramID(const unsigned int& programID);
	void SetRenderMode(const int& value);
	void SetDrawInfo(const ModelDrawInfo& mdi);
	void Draw();

	const bool SetEnableOffsetAlongNormal(const bool& value);
	const bool SetVertexOffset(const float& value);
	const bool SetWorldMatrices(const std::vector<Matrix4D>& matrices);
};

