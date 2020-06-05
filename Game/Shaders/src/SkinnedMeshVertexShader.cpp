#include "../include/SkinnedMeshVertexShader.h"
#include <BellEngineID.h>

const bool SkinnedMeshVertexShader::GetUniformLocations()
{
	this->matBonesArray_ul = glGetUniformLocation(this->programID, "matrixBonesArray");
	this->modelMatrix_ul = glGetUniformLocation(this->programID, "matrixModel");
	this->enableOffsetAlongNormal_ul = glGetUniformLocation(this->programID, "shaderBasicVertexInfo.offsetAlongNormal");
	this->vertexOffset_ul = glGetUniformLocation(this->programID, "shaderBasicVertexInfo.offset");

	return true;
}

SkinnedMeshVertexShader::SkinnedMeshVertexShader()
{
	matBonesArray_ul = renderMode = programID = modelMatrix_ul = enableOffsetAlongNormal_ul = vertexOffset_ul = 0;
}

SkinnedMeshVertexShader::~SkinnedMeshVertexShader()
{

}

const unsigned long long SkinnedMeshVertexShader::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_GLSHADER_VERTEX_SKINNED;
}

const std::string SkinnedMeshVertexShader::GetType()
{
	return "SkinnedMeshShader";
}

const std::string SkinnedMeshVertexShader::GetName()
{
	return this->name;
}

void SkinnedMeshVertexShader::SetName(const std::string& name)
{
	this->name = name;
}

const bool SkinnedMeshVertexShader::Initialize()
{
	bool rc = true;

	GetUniformLocations();

	return rc;
}

const bool SkinnedMeshVertexShader::SetBoneMatrices(const std::vector<Matrix4D>& matrices)
{
	glUniformMatrix4fv(matBonesArray_ul, (GLsizei)matrices.size(), GL_FALSE, glm::value_ptr(matrices[0]));
	return !matrices.empty();
}

const bool SkinnedMeshVertexShader::SetModelMatrix(const Matrix4D& value)
{
	glUniformMatrix4fv(this->modelMatrix_ul, 1, GL_FALSE, glm::value_ptr(value));
	return true;
}

const bool SkinnedMeshVertexShader::SetEnableOffsetAlongNormal(const bool& value)
{
	glUniform1i(this->enableOffsetAlongNormal_ul, value ? 1 : 0);

	return true;
}

const bool SkinnedMeshVertexShader::SetVertexOffset(const float& value)
{
	glUniform1f(this->vertexOffset_ul, value);

	return true;
}

void SkinnedMeshVertexShader::SetProgramID(const unsigned int& programID)
{
	glUseProgram(programID);

	if (this->programID != programID)
	{
		this->programID = programID;
		GetUniformLocations();
	}

	this->programID = programID;
}

void SkinnedMeshVertexShader::SetRenderMode(const int& value)
{
	this->renderMode = value;
}

void SkinnedMeshVertexShader::SetDrawInfo(const ModelDrawInfo& mdi)
{
	this->mdi = mdi;
}

void SkinnedMeshVertexShader::Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, this->renderMode);
	glBindVertexArray(this->mdi.VAO_ID);
	glDrawElements(GL_TRIANGLES, (GLsizei)mdi.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
