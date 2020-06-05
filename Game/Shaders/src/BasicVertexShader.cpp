#include "..\include\BasicVertexShader.h"
#include <BellEngineID.h>

const bool BasicVertexShader::GetUniformLocations()
{
	this->modelMatrix_ul = glGetUniformLocation(this->programID, "matrixModel");
	this->enableOffsetAlongNormal_ul = glGetUniformLocation(this->programID, "shaderBasicVertexInfo.offsetAlongNormal");
	this->vertexOffset_ul = glGetUniformLocation(this->programID, "shaderBasicVertexInfo.offset");

	return true;
}

BasicVertexShader::BasicVertexShader()
{
	renderMode = programID = modelMatrix_ul = enableOffsetAlongNormal_ul = vertexOffset_ul = 0;
}

BasicVertexShader::~BasicVertexShader()
{

}

const unsigned long long BasicVertexShader::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_GLSHADER_VERTEX_BASIC;
}

const std::string BasicVertexShader::GetType()
{
	return "BasicVertexShader";
}

const std::string BasicVertexShader::GetName()
{
	return this->name;
}

void BasicVertexShader::SetName(const std::string& name)
{
	this->name = name;
}

const bool BasicVertexShader::Initialize()
{
	bool rc = true;

	GetUniformLocations();

	return rc;
}

const bool BasicVertexShader::SetModelMatrix(const Matrix4D& value)
{
	glUniformMatrix4fv(this->modelMatrix_ul, 1, GL_FALSE, glm::value_ptr(value));
	return true;
}

const bool BasicVertexShader::SetEnableOffsetAlongNormal(const bool& value)
{
	glUniform1i(this->enableOffsetAlongNormal_ul, value ? 1 : 0);

	return true;
}

const bool BasicVertexShader::SetVertexOffset(const float& value)
{
	glUniform1f(this->vertexOffset_ul, value);

	return true;
}

void BasicVertexShader::SetProgramID(const unsigned int& programID)
{
	glUseProgram(programID);

	if (this->programID != programID)
	{
		this->programID = programID;
		GetUniformLocations();
	}

	this->programID = programID;
}

void BasicVertexShader::SetRenderMode(const int& value)
{
	this->renderMode = value;
}

void BasicVertexShader::SetDrawInfo(const ModelDrawInfo& mdi)
{
	this->mdi = mdi;
}

void BasicVertexShader::Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, this->renderMode);
	glBindVertexArray(this->mdi.VAO_ID);
	glDrawElements(GL_TRIANGLES, (GLsizei)mdi.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
