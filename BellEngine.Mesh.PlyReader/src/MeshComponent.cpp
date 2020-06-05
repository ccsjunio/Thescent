#include "MeshComponent.h"
#include <BellEngineID.h>

MeshComponent::MeshComponent()
{
	levelOfDetail = 0u;
	isActive = true;
	allowMeshTransform = false;
}

MeshComponent::~MeshComponent()
{
	Dispose();
}

const bool MeshComponent::AddTexture(ITexture* texture)
{
	std::vector<ITexture*>::iterator it = std::find(textures.begin(), textures.end(), texture);
	if (it == textures.end())
	{
		textures.push_back(texture);
		return true;
	}

	return false;
}

const bool MeshComponent::RemoveTexture(ITexture* texture)
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

const bool MeshComponent::GetTextures(std::vector<ITexture*>& textures)
{
	textures = this->textures;
	return !textures.empty();
}

const bool MeshComponent::AddTextureToMesh(IMesh* mesh, ITexture* texture)
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

const bool MeshComponent::RemoveTextureFromMesh(IMesh* mesh, ITexture* texture)
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

const bool MeshComponent::GetMeshTextures(IMesh* mesh, std::vector<ITexture*>& textures)
{
	auto it = meshTextureMap.find(mesh);
	if (it != meshTextureMap.end())
	{
		textures = (*it).second;
		return true;
	}

	return false;
}


void MeshComponent::SetLevelOfDetail(const unsigned int& levelOfDetail)
{
	this->levelOfDetail = levelOfDetail;
}

unsigned int MeshComponent::GetLevelOfDetail()
{
	return levelOfDetail;
}

const bool MeshComponent::IsActive()
{
	return isActive;
}

void MeshComponent::Activate(const bool& value)
{
	isActive = value;
}

const bool MeshComponent::AllowMeshTranformations()
{
	return allowMeshTransform;
}

void MeshComponent::ToggleMeshTransformations(const bool& value)
{
	this->allowMeshTransform = value;
}

const bool MeshComponent::AddMesh(IMesh* mesh)
{
	std::vector<IMesh*>::iterator it = std::find(meshes.begin(), meshes.end(), mesh);
	if (it == meshes.end())
	{
		meshes.push_back(mesh);
		return true;
	}

	return false;
}

const bool MeshComponent::RemoveMesh(IMesh* mesh)
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

const bool MeshComponent::GetMeshes(std::vector<IMesh*>& meshes)
{
	meshes = this->meshes;

	return meshes.size() != 0;
}

void MeshComponent::Dispose()
{
	DisposeVector(meshes);
}

const std::string MeshComponent::GetName()
{
	return this->name;
}

void MeshComponent::SetName(const std::string& name)
{
	this->name = name;
}

const std::string MeshComponent::GetType()
{
	return "MeshComponent";
}

const unsigned long long MeshComponent::GetTypeID()
{
	return ENGINE_OBJ_ID_MSH_MESHCOMPONENT;
}

ICloneable* MeshComponent::Clone()
{
	MeshComponent* clone = new MeshComponent();

	clone->allowMeshTransform = allowMeshTransform;
	clone->SetName(GetName());

	for (IMesh* mesh : meshes)
	{
		clone->AddMesh((IMesh*)mesh->Clone());
	}

	return clone;
}
