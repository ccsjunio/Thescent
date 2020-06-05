#include "MeshComponentFactory.h"
#include <MeshComponent.h>
#include <EngineFolders.h>

IMeshComponent* MeshComponentFactory::CreateMeshComponent(ParameterContainer& parameters)
{
	bool rc = false;
	Parameter p;
	std::string meshPath, texturesPath, name;
	bool allowMeshTransform = false;
	unsigned int levelOfDetail = 0;
	unsigned int textureType = 0;

	rc = parameters.FindParameterByName("Name", p);
	if (rc) name = p.value;

	rc = parameters.FindParameterByName("MeshFile", p);
	if (!rc) return nullptr;
	meshPath = EngineFolders::GetModelsPath() + p.value;

	rc = parameters.FindParameterByName("TextureType", p);
	if (rc) textureType = strtoul(p.value.c_str(), NULL, 0);

	rc = parameters.FindParameterByName("TexturesPath", p);
	if (rc) texturesPath = EngineFolders::GetTexturesPath() + p.value;

	rc = parameters.FindParameterByName("AllowMeshTransform", p);
	if (rc) allowMeshTransform = atoi(p.value.c_str()) != 0;

	rc = parameters.FindParameterByName("LevelOfDetail", p);
	if (rc) levelOfDetail = strtoul(p.value.c_str(), NULL, 0);

	/*Checks if the mesh was already loaded*/
	std::vector<IMesh*> meshes;
	if (meshReader->WasMeshRead(meshPath))
	{
		for (auto element : *meshMap)
		{
			if (element.second->GetPath() == meshPath)
			{
				meshes.push_back(element.second);
			}
		}
	}
	else
	{
		/*Reads mesh file in Mesh classes*/
		if (!meshReader->ReadMesh(meshPath, meshes))
		{
			std::string err = ("Could not load meshes.\n");
			SetLastErrorString(err);
			return nullptr;
		}

		for (auto mesh : meshes)
		{
			meshMap->operator[](mesh->GetName()) = mesh;
		}
	}

	std::vector<ITexture*> textures;
	if (!texturesPath.empty())
	{		
		if (meshReader->WasTextureRead(texturesPath))
		{
			for (auto element : *this->textures)
			{
				if (element->path.rfind(texturesPath) == 0)
				{
					textures.push_back(element);
				}
			}
		}
		else
		{
			/*Reads textures in Texture classes*/
			if (!meshReader->ReadTextures(texturesPath, textures, textureType))
			{
				std::string err = "Mesh at " + texturesPath + " has no textures.";
				SetLastErrorString(err);
				return nullptr;
			}

			for (auto texture : textures)
			{
				this->textures->push_back(texture);
			}
		}

		///*Puts textures in meshes*/
		//for (size_t i = 0; i < meshes.size(); i++)
		//{
		//	IMesh* mesh = meshes[i];
		//	mesh->SetName(mesh->GetName() + "_" + std::to_string(i));

		//	for (size_t textIndex = 0; textIndex < textures.size(); textIndex++)
		//	{
		//		mesh->AddTexture(textures[textIndex]);
		//	}
		//}
	}

	MeshComponent* mC = new MeshComponent();
	mC->ToggleMeshTransformations(allowMeshTransform);
	mC->SetName(name);
	for (ITexture* t : textures)
	{
		mC->AddTexture(t);
	}
	for (IMesh* mesh : meshes)
	{
		mC->AddMesh(mesh);

		for (ITexture* t : textures)
		{
			mC->AddTextureToMesh(mesh, t);
		}
	}

	return mC;
}

MeshComponentFactory::MeshComponentFactory()
{
	meshMap = nullptr;
	meshReader = new MeshReader();
}

MeshComponentFactory::~MeshComponentFactory()
{
	Dispose();
}

IMeshComponent* MeshComponentFactory::Create(ParameterContainer& parameters)
{
	bool rc = false;
	Parameter p;

	if (parameters.FindParameterByName("Type", p))
	{
		if (p.value == "MeshComponent")
		{
			return CreateMeshComponent(parameters);
		}
	}

	return nullptr;
}

const bool MeshComponentFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	bool rc = false;
	Parameter p;
	
	if (input.GetCount() == 0)
	{
		return true;
	}
	else
	{
		if (input.FindParameterByName("meshMap", p))
		{
			meshMap = (std::map<std::string, IMesh*>*)p.valuePtr;
		}

		if (input.FindParameterByName("textures", p))
		{
			textures = (std::vector<ITexture*>*)p.valuePtr;
		}

		return true;
	}

	return false;
}

void MeshComponentFactory::Dispose()
{
	DisposePointer(meshReader);
}
