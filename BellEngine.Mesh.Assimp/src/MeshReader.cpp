#include "MeshReader.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <Mesh.h>
#include <StringHelpers.h>
#include <EngineFolders.h>
#include <Texture2D.h>
#include <TextureSkybox.h>
#if __cplusplus >= 201703L
	#include <filesystem>
#endif

const bool MeshReader::ReadMesh(const std::string& path, std::vector<IMesh*>& meshes)
{
	Assimp::Importer importer;
	unsigned int Flags = aiProcess_Triangulate |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace;

	const aiScene* assimpScene = importer.ReadFile(path.c_str(), Flags);
	if (assimpScene)
	{
		for (unsigned int meshIndex = 0; meshIndex < assimpScene->mNumMeshes; meshIndex++)
		{
			IMesh* mesh = new Mesh();
			mesh->SetPath(path);
			mesh->SetName(StringHelpers::GetFileNameFromFilePath(path));
			mesh->SetName(StringHelpers::GetNameWithoutExtension(mesh->GetName()) + "_" + std::to_string(meshIndex));
		
			aiMesh* aimesh = assimpScene->mMeshes[meshIndex];
			if (aimesh)
			{
			/*	std::string meshname = aimesh->mName.C_Str();
				if(meshname.empty())
				{
					mesh->SetName(StringHelpers::GetFileNameFromFilePath(path));
					mesh->SetName(StringHelpers::GetNameWithoutExtension(mesh->GetName()) + "_" + std::to_string(meshIndex));
				}
				else
				{
					mesh->SetName(meshname);
				}*/

				/*std::vector<std::array<float,4>> boneids; boneids.resize(aimesh->mNumVertices);
				std::vector<std::array<float, 4>> boneweights; boneweights.resize(aimesh->mNumVertices);
				std::map<std::string, unsigned int> m_mapBoneNameToBoneIndex;
				unsigned int nBones = 0;
				if (aimesh->HasBones())
				{
					for (unsigned int boneIndex = 0; boneIndex != aimesh->mNumBones; boneIndex++)
					{
						std::string BoneName(aimesh->mBones[boneIndex]->mName.data);
						unsigned int BoneIndex = 0;

						std::map<std::string, unsigned int>::iterator it = m_mapBoneNameToBoneIndex.find(BoneName);
						if (it == m_mapBoneNameToBoneIndex.end())
						{
							BoneIndex = nBones;
							nBones++;
							m_mapBoneNameToBoneIndex[BoneName] = BoneIndex;
						}
						else
						{
							BoneIndex = it->second;
						}

						for (unsigned int WeightIndex = 0; WeightIndex != aimesh->mBones[boneIndex]->mNumWeights; WeightIndex++)
						{
							unsigned int VertexID = aimesh->mBones[boneIndex]->mWeights[WeightIndex].mVertexId;
							float Weight = aimesh->mBones[boneIndex]->mWeights[WeightIndex].mWeight;

							for (unsigned int k = 0; k < 4; k++)
							{
								boneids[VertexID][k] = (float)BoneIndex;
								boneweights[VertexID][k] = (float)Weight;
							}
						}
					}
				}*/

				//VERTICES
				for (unsigned int vertIndex = 0; vertIndex < aimesh->mNumVertices; vertIndex++)
				{
					Vertex3D* vertex = new Vertex3D();

					aiVector3D aivector3DVert = aimesh->mVertices[vertIndex];
					vertex->x = aivector3DVert.x;
					vertex->y = aivector3DVert.y;
					vertex->z = aivector3DVert.z;

					aiVector3D aivector3D_Normal = aimesh->mNormals[vertIndex];
					vertex->nx = aivector3D_Normal.x;
					vertex->ny = aivector3D_Normal.y;
					vertex->nz = aivector3D_Normal.z;

					if (aimesh->HasTextureCoords(0))
					{
						vertex->u0 = aimesh->mTextureCoords[0][vertIndex].x;
						vertex->v0 = aimesh->mTextureCoords[0][vertIndex].y;
					}

					if (aimesh->HasTangentsAndBitangents())
					{
						vertex->tx = aimesh->mTangents[vertIndex].x;
						vertex->ty = aimesh->mTangents[vertIndex].y;
						vertex->tz = aimesh->mTangents[vertIndex].z;

						vertex->bx = aimesh->mBitangents[vertIndex].x;
						vertex->by = aimesh->mBitangents[vertIndex].y;
						vertex->bz = aimesh->mBitangents[vertIndex].z;
					}

					/*if (aimesh->HasBones())
					{
						for (unsigned int bn = 0; bn < 4; bn++)
						{
							vertex->boneID[bn] = boneids[vertIndex][bn];
							vertex->boneWeights[bn] = boneweights[vertIndex][bn];
						}
					}*/

					mesh->AddVertex(vertex);
				}

				//TRIANGLES
				for (unsigned int triangleIndex = 0; triangleIndex < aimesh->mNumFaces; triangleIndex++)
				{
					aiFace aiface = aimesh->mFaces[triangleIndex];

					Vertex3D* plyTriangleVertex1 = mesh->FindVertexByIndex(aiface.mIndices[0]);
					plyTriangleVertex1->index = aiface.mIndices[0];
					Vertex3D* plyTriangleVertex2 = mesh->FindVertexByIndex(aiface.mIndices[1]);
					plyTriangleVertex2->index = aiface.mIndices[1];
					Vertex3D* plyTriangleVertex3 = mesh->FindVertexByIndex(aiface.mIndices[2]);
					plyTriangleVertex3->index = aiface.mIndices[2];

					Triangle3D* triangle = new Triangle3D(*plyTriangleVertex1, *plyTriangleVertex2, *plyTriangleVertex3);
					mesh->AddTriangle(triangle);
				}

				//BONES
				/*for (unsigned int boneIndex = 0; boneIndex < aimesh->mNumBones; boneIndex++)
				{
					IMesh::BoneInfo_s* boneInfo = new IMesh::BoneInfo_s();

					boneInfo->boneName = aimesh->mBones[boneIndex]->mName.data;
					boneInfo->BoneOffset = AssimpMatrixToGLMMatrix(aimesh->mBones[boneIndex]->mOffsetMatrix);

					for (unsigned int weightIndex = 0; weightIndex < aimesh->mBones[boneIndex]->mNumWeights; weightIndex++)
					{
						boneInfo->AppendBoneData(aimesh->mBones[boneIndex]->mWeights[weightIndex].mVertexId, aimesh->mBones[boneIndex]->mWeights[weightIndex].mWeight);
					}

					mesh->bones.push_back(boneInfo);
				}*/

				meshes.push_back(mesh);
			}
			else
			{
				//This mesh is invalid
				return false;
			}
		}
	}
	else
	{
		SetLastErrorString("Could not load mesh file!");
		return false;
	}

	meshesRead.push_back(path);
	return true;
}

const bool MeshReader::ReadTextures(const std::string& path, std::vector<ITexture*>& textures, const unsigned int& textureType)
{
	bool rc = false;

	std::string nameAndExt = StringHelpers::GetFileNameFromFilePath(path);
	std::string name = StringHelpers::GetNameWithoutExtension(nameAndExt);
	std::string folder = EngineFolders::GetTexturesPath() + name + "\\";

#if __cplusplus >= 201703L
	rc = std::filesystem::exists(folder);
	if (!rc) return rc;

	/*Basic texture*/
	if (textureType == 0)
	{
		/*Every file generates a texture.*/
		for (auto it : std::filesystem::recursive_directory_iterator(folder))
		{
			if (it.is_regular_file())
			{
				std::string filepath = it.path().string();
				std::string name = StringHelpers::GetFileNameFromFilePath(filepath);
				name = StringHelpers::GetNameWithoutExtension(name);

				ITexture* texture = new Texture2D();
				texture->SetName(name);
				texture->path = filepath;

				textures.push_back(texture);
			}
		}
	}
	/*Skybox texture*/
	else if (textureType == 1)
	{
		TextureSkybox* texture = new TextureSkybox();
		texture->SetName("SkyboxTexture");

		/*Six files generate a texture*/
		unsigned int i = 0;
		for (auto it : std::filesystem::recursive_directory_iterator(folder))
		{
			if (it.is_regular_file())
			{
				switch (i)
				{
				case 0: texture->SetPositiveX(it.path().string()); break;
				case 1: texture->SetNegativeX(it.path().string()); break;
				case 2: texture->SetPositiveY(it.path().string()); break;
				case 3: texture->SetNegativeY(it.path().string()); break;
				case 4: texture->SetPositiveZ(it.path().string()); break;
				case 5: texture->SetNegativeZ(it.path().string()); break;
				default:
					break;
				}

				i++;

				/*Only reads first 6 files.*/
				if (i > 5)
				{
					break;
				}
			}
		}

		textures.push_back(texture);
	}
#endif	

	rc = textures.size() != 0;
	if (rc)
	{
		texturesRead.push_back(path);
	}

	return rc;
}

Matrix4D MeshReader::AssimpMatrixToGLMMatrix(const aiMatrix4x4& mat)
{
	return Matrix4D(mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4);
}

const bool MeshReader::WasMeshRead(const std::string& path)
{
	auto it = std::find(meshesRead.begin(), meshesRead.end(), path);

	return it != meshesRead.end();
}

const bool MeshReader::WasTextureRead(const std::string& path)
{
	auto it = std::find(texturesRead.begin(), texturesRead.end(), path);

	return it != texturesRead.end();
}

MeshReader::MeshReader()
{
}

MeshReader::~MeshReader()
{
}
