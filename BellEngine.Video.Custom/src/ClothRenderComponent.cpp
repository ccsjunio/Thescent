#include "ClothRenderComponent.h"
#include <Triangle3D.h>
#include <BellEngineID.h>

ClothRenderComponent::ClothRenderComponent()
{
	isActive = true;
	baseSpecularColor = currentDiffuseColor = baseDiffuseColor = currentSpecularColor = Vector4D();
	isBaseDiffuseSet = isBaseSpecularSet = false;
	isVisible = true;
	renderMode = alternateRenderMode = 0;
}

ClothRenderComponent::~ClothRenderComponent()
{
}

const bool ClothRenderComponent::GetClothMDI(const size_t& rowCount, const size_t& columnCount, const std::vector<Vector3D>& nodePositions, ModelDrawInfo& modelDrawInfo)
{
	modelDrawInfo = ModelDrawInfo();

	if (nodePositions.empty() || rowCount == 0 || columnCount == 0)
	{
		return false;
	}

	size_t nodeCount = nodePositions.size();

	std::vector<Vertex3D> vertices;
	vertices.resize(nodeCount);

	for (size_t i = 0; i < nodeCount; i++)
	{
		Vertex3D vert = Vertex3D();
		vert.x = nodePositions[i].x;
		vert.y = nodePositions[i].y;
		vert.z = nodePositions[i].z;
		vert.nx = 0.0f;
		vert.ny = 0.0f;
		vert.nz = -1.0f;

		vertices[i] = vert;
	}

	size_t triangleCount = (rowCount - 1) * (columnCount - 1) * 2;

	std::vector<Triangle3D> triangles;
	triangles.resize(triangleCount);

	size_t triangleIndex = 0;
	size_t vertAIdx, vertBIdx, vertCIdx;
	Triangle3D triangle = Triangle3D();;
	for (size_t columnIndex = 0; columnIndex < columnCount - 1; columnIndex++)
	{
		for (size_t rowIndex = 0; rowIndex < rowCount; rowIndex++)
		{
			if (rowIndex - 1 >= 0)
			{
				vertAIdx = rowIndex + columnIndex * rowCount;
				vertBIdx = rowIndex + (columnIndex + 1) * rowCount;
				vertCIdx = (rowIndex - 1) + (columnIndex + 1) * rowCount;

				triangle = Triangle3D(vertices[vertAIdx], vertices[vertBIdx], vertices[vertCIdx]);
				triangles[triangleIndex] = triangle;
				triangleIndex++;
			}

			else if (rowIndex + 1 < rowCount)
			{
				vertAIdx = rowIndex + columnIndex * rowCount;
				vertBIdx = (rowIndex + 1) + (columnIndex * rowCount);
				vertCIdx = rowIndex + (columnIndex + 1) * rowCount;

				triangle = Triangle3D(vertices[vertAIdx], vertices[vertBIdx], vertices[vertCIdx]);
				triangles[triangleIndex] = triangle;
				triangleIndex++;
			}
		}
	}

	return true;
}
const bool ClothRenderComponent::GetClothTriangles(const size_t& rowCount, const size_t& columnCount, const std::vector<Vector3D>& nodePositions, std::vector<Triangle3D>& triangles)
{
	if (nodePositions.empty() || rowCount == 0 || columnCount == 0)
	{
		return false;
	}

	size_t nodeCount = nodePositions.size();

	std::vector<Vertex3D> vertices;
	vertices.resize(nodeCount);

	for (size_t i = 0; i < nodeCount; i++)
	{
		Vertex3D vert = Vertex3D();
		vert.x = nodePositions[i].x;
		vert.y = nodePositions[i].y;
		vert.z = nodePositions[i].z;
		vert.nx = 0.0f;
		vert.ny = 0.0f;
		vert.nz = -1.0f;

		vertices[i] = vert;
	}

	size_t triangleCount = (rowCount - 1) * (columnCount - 1) * 2;
		
	size_t triangleIndex = 0;
	size_t vertAIdx = 0;
	size_t vertBIdx = 0; 
	size_t vertCIdx = 0;
	Triangle3D triangle = Triangle3D();;

	for (size_t i = 0; i < columnCount - 1; i++)
	{
		for (size_t j = 0; j < rowCount; j++)
		{
			if (j + 1 < rowCount && i + 1 < columnCount)
			{
				vertAIdx = j + i * rowCount;
				vertBIdx = (j + 1) + (i * rowCount);
				vertCIdx = j + (i + 1) * rowCount;

				triangle = Triangle3D(vertices[vertAIdx], vertices[vertBIdx], vertices[vertCIdx]);
				triangles.push_back(triangle);
			}

			if (j - 1 >= 0 && i + 1 < columnCount)
			{
				vertAIdx = j + i * rowCount;
				vertBIdx = j + (i + 1) * rowCount;
				vertCIdx = (j - 1) + (i + 1) * rowCount;

				triangle = Triangle3D(vertices[vertAIdx], vertices[vertBIdx], vertices[vertCIdx]);
				triangles.push_back(triangle);
			}
		}
	}

	return true;
}
const bool ClothRenderComponent::IsActive()
{
	return isActive;
}
void ClothRenderComponent::Activate(const bool& value)
{
	isActive = value;
}
const Vector4D ClothRenderComponent::GetBaseDiffuseColor()
{
	return this->baseDiffuseColor;
}
const Vector4D ClothRenderComponent::GetBaseSpecularColor()
{
	return this->baseSpecularColor;
}
void ClothRenderComponent::SetDiffuse(const Vector4D& color)
{
	this->currentDiffuseColor = color;

	if (!isBaseDiffuseSet)
	{
		this->baseDiffuseColor = color;
		isBaseDiffuseSet = true;
	}
}
const Vector4D ClothRenderComponent::GetDiffuse()
{
	return currentDiffuseColor;
}
void ClothRenderComponent::SetSpecular(const Vector4D& color)
{
	this->currentSpecularColor = color;

	if (!isBaseSpecularSet)
	{
		this->baseSpecularColor = color;
		isBaseSpecularSet = true;
	}
}
const Vector4D ClothRenderComponent::GetSpecular()
{
	return currentSpecularColor;
}
const bool ClothRenderComponent::IsVisible()
{
	return isVisible;
}
void ClothRenderComponent::SetVisibility(const bool& option)
{
	isVisible = option;
}
void ClothRenderComponent::SetRenderMode(const int& renderMode)
{
	this->renderMode = renderMode;
}
const int ClothRenderComponent::GetRenderMode()
{
	return renderMode;
}
void ClothRenderComponent::SetAlternateRenderMode(const int& renderMode)
{
	this->alternateRenderMode = renderMode;
}
const int ClothRenderComponent::GetAlternateRenderMode()
{
	return this->alternateRenderMode;
}
const bool ClothRenderComponent::GetShaderFeatures(std::vector<ShaderFeature*>& shaderFeatures)
{
	shaderFeatures = this->shaderFeatures;
	return !shaderFeatures.empty();
}
const bool ClothRenderComponent::HasShaderFeature(const std::string& name)
{
	for (ShaderFeature*& sf : shaderFeatures)
	{
		if (sf->GetName() == name)
		{
			return true;
		}
	}

	return false;
}
ShaderFeature* ClothRenderComponent::GetShaderFeature(const std::string& name)
{
	for (ShaderFeature*& sf : shaderFeatures)
	{
		if (sf->GetName() == name)
		{
			return sf;
		}
	}

	return nullptr;
}
const bool ClothRenderComponent::Append(ShaderFeature* sf)
{
	if (sf)
	{
		auto it = std::find(shaderFeatures.begin(), shaderFeatures.end(), sf);
		if (it == shaderFeatures.end())
		{
			shaderFeatures.push_back(sf);
			return true;
		}
	}

	return false;
}
const bool ClothRenderComponent::Remove(ShaderFeature* sf)
{
	if (sf)
	{
		auto it = std::find(shaderFeatures.begin(), shaderFeatures.end(), sf);
		if (it != shaderFeatures.end())
		{
			shaderFeatures.erase(it);
			return true;
		}
	}

	return false;
}
const std::string ClothRenderComponent::GetType()
{
	return "ClothRenderComponent";
}
const unsigned long long ClothRenderComponent::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_CLOTHRENDERCOMPONENT;
}
const std::string ClothRenderComponent::GetName()
{
	return this->name;
}
void ClothRenderComponent::SetName(const std::string& name)
{
	this->name = name;
}
ICloneable* ClothRenderComponent::Clone()
{
	ClothRenderComponent* clone = new ClothRenderComponent();

	clone->renderMode = renderMode;
	clone->SetVisibility(isVisible);
	clone->baseDiffuseColor = baseDiffuseColor;
	clone->baseSpecularColor = baseSpecularColor;
	clone->currentDiffuseColor = currentDiffuseColor;
	clone->currentSpecularColor = currentSpecularColor;
	clone->isBaseDiffuseSet = isBaseDiffuseSet;
	clone->isBaseSpecularSet = isBaseSpecularSet;
	clone->SetName(GetName());

	for (auto sf : shaderFeatures)
	{
		clone->Append((ShaderFeature*)sf->Clone());
	}

	return clone;
}
void ClothRenderComponent::Dispose()
{
	DisposeVector(shaderFeatures);
}
