#include "ParticleMeshComponentFactory.h"
#include "ParticleMeshComponent.h"
#include <EngineFolders.h>

IParticleMeshComponent* ParticleMeshComponentFactory::CreateParticleMeshComponent(ParameterContainer& parameters)
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

	rc = parameters.FindParameterByName("TexturesPath", p);
	if (rc) texturesPath = EngineFolders::GetTexturesPath() + p.value;
		
	/*Checks if the mesh was already loaded*/
	std::vector<IMesh*> meshes;
	if (meshReader->WasMeshRead(meshPath))
	{
		if (findMeshByPathFunc)
		{
			findMeshByPathFunc(meshPath, meshes);
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

		if (addMeshFunc)
		{
			for (auto mesh : meshes)
			{
				addMeshFunc(mesh);
			}
		}
	}

	std::vector<ITexture*> textures;
	if (!texturesPath.empty())
	{
		if (meshReader->WasTextureRead(texturesPath))
		{
			if (findTextureByNameFunc)
			{
				findTextureByNameFunc(texturesPath, textures);
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

			if (addTextureFunc)
			{
				for (auto texture : textures)
				{
					addTextureFunc(texture);
				}
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

	ParticleMeshComponent* mC = new ParticleMeshComponent();
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

ParticleMeshComponentFactory::ParticleMeshComponentFactory()
{
	this->meshReader = new MeshReader();
}

ParticleMeshComponentFactory::~ParticleMeshComponentFactory()
{
	Dispose();
}

IParticleMeshComponent* ParticleMeshComponentFactory::Create(ParameterContainer& parameters)
{
	Parameter p;

	if (parameters.FindParameterByName("Type", p))
	{
		if (p.value == "ParticleMeshComponent")
		{
			return CreateParticleMeshComponent(parameters);
		}
	}

	return nullptr;
}

const bool ParticleMeshComponentFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return false;
}

void ParticleMeshComponentFactory::Dispose()
{
	DisposePointer(this->meshReader);
}
