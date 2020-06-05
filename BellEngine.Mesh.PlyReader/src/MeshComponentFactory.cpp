#include "MeshComponentFactory.h"
#include <MeshComponent.h>

MeshComponentFactory::MeshComponentFactory()
{
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
			std::string meshPath, texturesPath, name;
			bool allowMeshTransform = false;

			rc = parameters.FindParameterByName("Name", p);
			if (!rc) name = p.value;

			rc = parameters.FindParameterByName("MeshPath", p);
			if (!rc) return nullptr;
			meshPath = p.value;

			rc = parameters.FindParameterByName("TexturesPath", p);
			if (!rc) return nullptr;
			texturesPath = p.value;

			rc = parameters.FindParameterByName("AllowMeshTransform", p);
			if (!rc) allowMeshTransform = atoi(p.value.c_str()) != 0;

			/*Reads mesh file in Mesh classes*/
			std::vector<IMesh*> meshes;
			if (!meshReader->ReadMesh(meshPath, meshes))
			{
				std::string err = ("Could not load meshes.\n");
				SetLastErrorString(err);
				return nullptr;
			}

			/*Reads textures in Texture classes*/
			std::vector<ITexture*> textures;
			if (!meshReader->ReadTextures(texturesPath, textures))
			{
				std::string err = "Mesh at " + texturesPath + " has no textures.";
				SetLastErrorString(err);
				return nullptr;
			}

			/*Puts textures in meshes*/
			for (size_t i = 0; i < meshes.size(); i++)
			{
				IMesh* mesh = meshes[i];
				mesh->SetName(mesh->GetName() + "_" + std::to_string(i));

				for (size_t textIndex = 0; textIndex < textures.size(); textIndex++)
				{
					mesh->AddTexture(textures[textIndex]);
				}
			}

			MeshComponent* mC = new MeshComponent();
			mC->ToggleMeshTransformations(allowMeshTransform);
			mC->SetName(name);
			for (IMesh* mesh : meshes)
			{
				mC->AddMesh(mesh);
			}

			return mC;
		}
	}

	return nullptr;
}

const bool MeshComponentFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return false;
}

void MeshComponentFactory::Dispose()
{
	DisposePointer(meshReader);
}
