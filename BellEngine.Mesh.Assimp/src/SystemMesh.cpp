#include "SystemMesh.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <Mesh.h>
#include <StringHelpers.h>
#include <EngineFolders.h>
#include <Texture2D.h>
#include <MeshComponentFactory.h>
#include <ParticleMeshComponentFactory.h>
#if __cplusplus >= 201703L
	#include <filesystem>
#endif
#include <functional>

Matrix4D SystemMesh::AssimpMatrixToGLMMatrix(const aiMatrix4x4& mat)
{
	return Matrix4D(mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4);
}

SystemMesh::SystemMesh()
{
	this->isUpdateComplete = false;
	this->isActive = true;
	meshComponentFactory = new MeshComponentFactory();
	this->particleMeshComponentFactory = new ParticleMeshComponentFactory();
	if (this->particleMeshComponentFactory)
	{
		((ParticleMeshComponentFactory*)this->particleMeshComponentFactory)->addMeshFunc =
			std::bind(&SystemMesh::AddMesh, this, std::placeholders::_1);
		((ParticleMeshComponentFactory*)this->particleMeshComponentFactory)->addTextureFunc =
			std::bind(&SystemMesh::AddTexture, this, std::placeholders::_1);
		((ParticleMeshComponentFactory*)this->particleMeshComponentFactory)->findMeshByPathFunc =
			std::bind(&SystemMesh::FindMeshByPath, this, std::placeholders::_1, std::placeholders::_2);
		((ParticleMeshComponentFactory*)this->particleMeshComponentFactory)->findTextureByNameFunc =
			std::bind(&SystemMesh::FindTexturesByName, this, std::placeholders::_1, std::placeholders::_2);
	}

	ParameterContainer input, output;
	input.Add(Parameter("map", "meshMap", &meshMap, false));
	input.Add(Parameter("vector", "textures", &textures, false));

	meshComponentFactory->Initialize(input, output);
}

SystemMesh::~SystemMesh()
{
	Dispose();
}

const bool SystemMesh::AddTexture(ITexture* texture)
{
	auto it = std::find(textures.begin(), textures.end(), texture);
	if (it == textures.end())
	{
		textures.push_back(texture);
		return true;
	}

	return false;
}

const bool SystemMesh::FindTexturesByName(const std::string& name, std::vector<ITexture*>& tx)
{
	for (ITexture* texture : textures)
	{
		if (texture->GetName() == name)
		{
			tx.push_back(texture);
		}
	}

	return !tx.empty();
}

const bool SystemMesh::AddMesh(IMesh* mesh)
{
	auto it = meshMap.find(mesh->GetName());
	if (it == meshMap.end())
	{
		meshMap[mesh->GetName()] = mesh;
		return true;
	}

	return false;
}

const bool SystemMesh::FindMeshesByPath(const std::string& path, std::vector<IMesh*>& ms)
{
	for (auto element : meshMap)
	{
		if (element.second->GetPath() == path)
		{
			ms.push_back(element.second);
		}
	}

	return !ms.empty();
}

const bool SystemMesh::IsUpdateComplete()
{
	return this->isUpdateComplete;
}

const bool SystemMesh::GetMeshes(std::map<std::string, IMesh*>& meshMap)
{
	this->meshMap = meshMap;

	return !this->meshMap.empty();
}

const bool SystemMesh::IsActive()
{
	return isActive;
}

void SystemMesh::Activate(const bool& value)
{
	this->isActive = value;
}

IFactory<IMeshComponent>* SystemMesh::GetMeshComponentFactory()
{
	IFactory<IMeshComponent>* ptr = this->meshComponentFactory;

	return ptr;
}

IFactory<IParticleMeshComponent>* SystemMesh::GetParticleMeshComponentFactory()
{
	return particleMeshComponentFactory;
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
	DisposePointer(particleMeshComponentFactory);
}
