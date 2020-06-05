#include "ParticleMeshComponent.h"
#include <BellEngineID.h>

ParticleMeshComponent::ParticleMeshComponent()
{	
	isActive.store(true);
}

ParticleMeshComponent::~ParticleMeshComponent()
{
	Dispose();
}

const bool ParticleMeshComponent::IsActive()
{
	return isActive.load();
}

void ParticleMeshComponent::Activate(const bool& value)
{
	isActive.store(value);
}

const bool ParticleMeshComponent::AddMesh(IMesh* mesh)
{
	std::vector<IMesh*>::iterator it = std::find(meshes.begin(), meshes.end(), mesh);
	if (it == meshes.end())
	{
		meshes.push_back(mesh);
		return true;
	}

	return false;
}

const bool ParticleMeshComponent::RemoveMesh(IMesh* mesh)
{
	std::vector<IMesh*>::iterator it = std::find(meshes.begin(), meshes.end(), mesh);
	if (it != meshes.end())
	{
		meshes.erase(it);

		DisposePointer(mesh);

		meshes.shrink_to_fit();
		return true;
	}

	return false;
}

const bool ParticleMeshComponent::GetMeshes(std::vector<IMesh*>& meshes)
{
	meshes = this->meshes;

	return meshes.size() != 0;
}

const bool ParticleMeshComponent::AddTexture(ITexture* texture)
{
	std::vector<ITexture*>::iterator it = std::find(textures.begin(), textures.end(), texture);
	if (it == textures.end())
	{
		textures.push_back(texture);
		return true;
	}

	return false;
}

const bool ParticleMeshComponent::RemoveTexture(ITexture* texture)
{
	std::vector<ITexture*>::iterator it = std::find(textures.begin(), textures.end(), texture);
	if (it != textures.end())
	{
		textures.erase(it);

		//DisposePointer(texture);
		return true;
	}

	return false;
}

const bool ParticleMeshComponent::GetTextures(std::vector<ITexture*>& textures)
{
	textures = this->textures;
	return !textures.empty();
}

const bool ParticleMeshComponent::AddTextureToMesh(IMesh* mesh, ITexture* texture)
{
	auto it = meshTextureMap.find(mesh);
	if (it == meshTextureMap.end())
	{
		std::vector<ITexture*> txts;
		txts.push_back(texture);
		meshTextureMap[mesh] = txts;
		return true;
	}
	else
	{
		std::vector<ITexture*> txts = meshTextureMap[mesh];
		auto txtit = std::find(txts.begin(), txts.end(), texture);
		if (txtit == txts.end())
		{
			meshTextureMap[mesh].push_back(texture);
		}
		else
		{
			return false;
		}
	}

	return false;
}

const bool ParticleMeshComponent::RemoveTextureFromMesh(IMesh* mesh, ITexture* texture)
{
	auto it = meshTextureMap.find(mesh);
	if (it != meshTextureMap.end())
	{
		auto txtit = std::find(meshTextureMap[mesh].begin(), meshTextureMap[mesh].end(), texture);
		if (txtit != meshTextureMap[mesh].end())
		{
			meshTextureMap[mesh].erase(txtit);
			return true;
		}
	}

	return false;
}

const bool ParticleMeshComponent::GetMeshTextures(IMesh* mesh, std::vector<ITexture*>& textures)
{
	auto it = meshTextureMap.find(mesh);
	if (it != meshTextureMap.end())
	{
		textures = (*it).second;
		return true;
	}

	return false;
}

void ParticleMeshComponent::SetLevelOfDetail(const unsigned int& levelOfDetail)
{
	this->levelOfDetail.store(levelOfDetail);
}

unsigned int ParticleMeshComponent::GetLevelOfDetail()
{
	return this->levelOfDetail.load();
}

const bool ParticleMeshComponent::AllowMeshTranformations()
{
	return this->allowMeshTransform;
}

void ParticleMeshComponent::ToggleMeshTransformations(const bool& value)
{
	this->allowMeshTransform.store(value);
}

void ParticleMeshComponent::Dispose()
{
	//DisposeVector(meshes);
}

const std::string ParticleMeshComponent::GetName()
{
	return this->name;
}

void ParticleMeshComponent::SetName(const std::string& name)
{
	this->name = name;
}

const std::string ParticleMeshComponent::GetType()
{
	return "ParticleMeshComponent";
}

const unsigned long long ParticleMeshComponent::GetTypeID()
{
	return ENGINE_OBJ_ID_MSH_PARTICLECOMPONENT;
}

ICloneable* ParticleMeshComponent::Clone()
{
	ParticleMeshComponent* clone = new ParticleMeshComponent();
	clone->SetName(name);

	for (IMesh* mesh : meshes)
	{
		clone->AddMesh((IMesh*)mesh->Clone());
	}

	return clone;
}
