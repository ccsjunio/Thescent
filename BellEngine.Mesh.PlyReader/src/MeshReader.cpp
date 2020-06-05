#include "MeshReader.h"
#include <sstream>
#include "Mesh.h"
#include <StringHelpers.h>
#include <EngineFolders.h>
#include <TextureHeightMap.h>
#include <Texture2D.h>
#if __cplusplus >= 201703L
	#include <filesystem>
#endif

const bool MeshReader::ReadMesh(const std::string& path, std::vector<IMesh*>& meshes)
{
	std::string input = StringHelpers::GetAllLinesFromFile(path);

	std::string nameAndExt = StringHelpers::GetFileNameFromFilePath(path);
	std::string name = StringHelpers::GetNameWithoutExtension(nameAndExt);
	std::string folder = EngineFolders::GetTexturesPath() + name + "\\";

#if __cplusplus >= 201703L
	if (!std::filesystem::exists(path))
	{
		SetLastErrorString("The path to this mesh does not exist.");
		return false;
	}
#endif

	Mesh* mesh = new Mesh();
	mesh->SetPath(path);
	mesh->SetName(name);

	std::stringstream ss(input);

	//Find textures
	std::string temp;
	while (ss >> temp)
	{
		if (temp == "TextureFile")
		{
			ITexture* texture = nullptr;

			ss >> temp;
			std::string textureFileName = temp;
			std::string texturePath = EngineFolders::GetModelsPath() + "\\" + textureFileName;

			if (textureFileName.rfind("HeightMap") != std::string::npos)
			{
				texture = new TextureHeightMap();
				texture->path = texturePath;

				//HeightMapMRatio
				ss >> temp;
				((TextureHeightMap*)texture)->ratio = (float)atof(temp.c_str());
				//Swizzle X
				ss >> temp;
				((TextureHeightMap*)texture)->swizzle.x = (float)(atof(temp.c_str()));
				//Swizzle Y
				ss >> temp;
				((TextureHeightMap*)texture)->swizzle.y = (float)(atof(temp.c_str()));
				//AllowOffsetIncrement
				ss >> temp;
				((TextureHeightMap*)texture)->allowOffsetIncrements = atoi(temp.c_str()) != 0;
				//TexturetOffset X
				ss >> temp;
				((TextureHeightMap*)texture)->offsetIncrement.x = (float)(atof(temp.c_str()));
				//TexturetOffset Y
				ss >> temp;
				((TextureHeightMap*)texture)->offsetIncrement.y = (float)(atof(temp.c_str()));

				mesh->AddTexture(texture);
			}
			else
			{
				texture = new Texture2D();
				texture->path = texturePath;

				mesh->AddTexture(texture);
			}
		}
	}

	ss.clear();
	ss.seekg(0, ss.beg);

	unsigned int vertexCount = 0;
	while (ss >> temp)
	{
		if (temp == "vertex") break;
	}
	ss >> vertexCount;

	unsigned int triangleCount = 0;
	while (ss >> temp)
	{
		if (temp == "face") break;
	}
	ss >> triangleCount;

	// Scan to end_header
	while (ss >> temp)
	{
		if (temp == "end_header") break;
	}

	for (unsigned int i = 0; i != vertexCount; i++)
	{
		Vertex3D* vertex = new Vertex3D();
		//Position
		ss >> vertex->x >> vertex->y >> vertex->z;
		//Normals
		ss >> vertex->nx >> vertex->ny >> vertex->nz;
		//Texture coordinates
		ss >> vertex->u0 >> vertex->v0;

		mesh->AddVertex(vertex);
	}

	for (unsigned int i = 0; i != triangleCount; i++)
	{
		unsigned int discard, indexOfVertexA, indexOfVertexB, indexOfVertexC;
		ss >> discard >> indexOfVertexA >> indexOfVertexB >> indexOfVertexC;

		// Get the vertices of the triangle
		Vertex3D* plyTriangleVertex1 = mesh->FindVertexByIndex(indexOfVertexA);
		plyTriangleVertex1->index = indexOfVertexA;
		Vertex3D* plyTriangleVertex2 = mesh->FindVertexByIndex(indexOfVertexB);
		plyTriangleVertex2->index = indexOfVertexB;
		Vertex3D* plyTriangleVertex3 = mesh->FindVertexByIndex(indexOfVertexC);
		plyTriangleVertex3->index = indexOfVertexC;

		Triangle3D* triangle = new Triangle3D(*plyTriangleVertex1, *plyTriangleVertex2, *plyTriangleVertex3);

		mesh->AddTriangle(triangle);
	}

	if (mesh)
	{
		meshes.push_back(mesh);
	}

	return meshes.size() != 0;
}

const bool MeshReader::ReadTextures(const std::string& path, std::vector<ITexture*>& textures)
{
	bool rc = false;



	return rc;
}

MeshReader::MeshReader()
{
}

MeshReader::~MeshReader()
{
}
