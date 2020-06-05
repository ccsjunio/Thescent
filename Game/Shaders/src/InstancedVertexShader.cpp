#include "..\include\InstancedVertexShader.h"
#include <BellEngineID.h>

const bool InstancedVertexShader::GetUniformLocations()
{	
	this->enableOffsetAlongNormal_ul = glGetUniformLocation(this->programID, "offsetAlongNormal");
	
	this->vertexOffset_ul = glGetUniformLocation(this->programID, "offset");

	this->matricesArray_ul.resize(max_matrices_size);
	char buf[255];
	for (size_t i = 0; i < this->max_matrices_size; i++)
	{
		sprintf_s(buf, 255, "instance_matrices[%llu]", i);
		this->matricesArray_ul[i] = glGetUniformLocation(this->programID, buf);
	}

	return true;
}

InstancedVertexShader::InstancedVertexShader()
{
	current_matrices_size = 0ull;
	renderMode = programID = matrices_ul = enableOffsetAlongNormal_ul = vertexOffset_ul = 0;
	max_matrices_size = 100ull;
}

InstancedVertexShader::~InstancedVertexShader()
{

}

const unsigned long long InstancedVertexShader::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_GLSHADER_VERTEXINSTANCE_0;
}

const std::string InstancedVertexShader::GetType()
{
	return "InstancedVertexShader";
}

const std::string InstancedVertexShader::GetName()
{
	return this->name;
}

void InstancedVertexShader::SetName(const std::string& name)
{
	this->name = name;
}

const bool InstancedVertexShader::Initialize()
{
	bool rc = true;

	GetUniformLocations();

	return rc;
}

const bool InstancedVertexShader::SetEnableOffsetAlongNormal(const bool& value)
{
	glUniform1i(this->enableOffsetAlongNormal_ul, value ? 1 : 0);

	return true;
}

const bool InstancedVertexShader::SetVertexOffset(const float& value)
{	
	glUniform1f(this->vertexOffset_ul, value);

	return true;
}

const bool InstancedVertexShader::SetWorldMatrices(const std::vector<Matrix4D>& matrices)
{
	current_matrices_size = matrices.size();

	if (current_matrices_size > max_matrices_size)
	{
		/*Can't draw more that the max capacity in the shader.*/
		return false;
	}

	for (size_t i = 0; i < current_matrices_size; i++)
	{		
		glUniformMatrix4fv(matricesArray_ul[i], 1, GL_FALSE, glm::value_ptr(matrices[i]));
	}

	return true;
}

void InstancedVertexShader::SetProgramID(const unsigned int& programID)
{
	glUseProgram(programID);

	if (this->programID != programID)
	{
		this->programID = programID;
		GetUniformLocations();
	}

	this->programID = programID;
}

void InstancedVertexShader::SetRenderMode(const int& value)
{
	this->renderMode = value;
}

void InstancedVertexShader::SetDrawInfo(const ModelDrawInfo& mdi)
{
	this->mdi = mdi;
}

void InstancedVertexShader::Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, this->renderMode);
	glBindVertexArray(this->mdi.VAO_ID);
	/*Instance rendering.*/
	glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)this->mdi.indices.size(), GL_UNSIGNED_INT, 0, this->current_matrices_size);
	glBindVertexArray(0);
}
