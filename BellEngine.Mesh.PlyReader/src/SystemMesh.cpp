#include "SystemMesh.h"
#include <Mesh.h>
#include <StringHelpers.h>
#include <EngineFolders.h>
#include <Texture2D.h>
#if __cplusplus >= 201703L
#include <filesystem>
#endif
#include <TextureHeightMap.h>
#include "MeshComponentFactory.h"

SystemMesh::SystemMesh()
{
	this->isUpdateComplete = false;
	this->isActive = true;
	meshComponentFactory = new MeshComponentFactory();

	ParameterContainer input, output;
	input.Add(Parameter("map", "meshMap", &meshMap, false));
	input.Add(Parameter("vector", "textures", &textures, false));

	meshComponentFactory->Initialize(input, output);
}

SystemMesh::~SystemMesh()
{
	Dispose();
}

const bool SystemMesh::GetMeshes(std::map<std::string, IMesh*>& meshMap)
{
	this->meshMap = meshMap;

	return !this->meshMap.empty();
}

const bool SystemMesh::IsUpdateComplete()
{
	return this->isUpdateComplete;
}

const bool SystemMesh::IsActive()
{
	return isActive;
}

void SystemMesh::Activate(const bool& value)
{
	this->isActive = value;
}

IFactory<IParticleMeshComponent>* SystemMesh::GetParticleMeshComponentFactory()
{
	return nullptr;
}

IFactory<IMeshComponent>* SystemMesh::GetMeshComponentFactory()
{
	IFactory<IMeshComponent>* ptr = this->meshComponentFactory;

	return ptr;
}

const bool SystemMesh::RegisterMesh(IMesh* mesh)
{
	bool rc = false;

	if (mesh)
	{
		auto it = meshMap.find(mesh->GetName());
		if (it == meshMap.end())
		{
			meshMap[mesh->GetName()] = mesh;
			rc = true;
		}
	}

	return rc;
}

const bool SystemMesh::Remove(const std::string& path)
{
	bool rc = false;

	std::vector<IMesh*> _meshes;
	rc = FindMeshByPath(path, _meshes);
	if (rc)
	{
		for (IMesh* mesh : _meshes)
		{
			auto it = meshMap.find(mesh->GetName());
			if (it != meshMap.end())
			{
				meshMap.erase(it);
				DisposePointer(it->second);
			}
			else
			{
				//Could not find mesh
			}
		}
	}

	return rc;
}

ITexture* SystemMesh::FindTextureByName(const std::string& name)
{
	ITexture* ptr = nullptr;

	for (ITexture* texture : textures)
	{
		if (texture->GetName() == name)
		{
			ptr = texture;
			break;
		}
	}

	return ptr;
}

IMesh* SystemMesh::FindMeshByName(const std::string& name)
{
	IMesh* ptr = nullptr;

	for (std::pair<std::string, IMesh*> element : meshMap)
	{
		if (element.second->GetName() == name)
		{
			ptr = element.second;
			break;
		}
	}

	return ptr;
}

const bool SystemMesh::FindMeshByPath(const std::string& path, std::vector<IMesh*>& meshes)
{
	bool rc = false;

	size_t startSz = meshes.size();

	for (std::pair<std::string, IMesh*> element : meshMap)
	{
		if (element.second->GetPath() == path)
		{
			meshes.push_back(element.second);
		}
	}

	return meshes.size() > startSz;
}

void SystemMesh::Update(const FrameTime& frameTime)
{
	if (!this->isActive) return;
	this->isUpdateComplete = false;

	SetFrameTime(frameTime);

	this->isUpdateComplete = true;
}

void SystemMesh::Dispose()
{
	DisposeVector(textures);
	DisposeMap(meshMap);
	DisposePointer(meshComponentFactory);
}
