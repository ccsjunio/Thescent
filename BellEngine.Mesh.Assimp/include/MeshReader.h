#pragma once
#include <assimp\matrix4x4.h>
#include <Vector>
#include <IMesh.h>
#include <ITexture.h>
#include <string>
#include <IExtendedInfo.h>

class MeshReader : public IExtendedInfo
{
private:
	std::vector<std::string> meshesRead;
	std::vector<std::string> texturesRead;

public:
	const bool ReadMesh(const std::string& path, std::vector<IMesh*>& meshes);
	const bool ReadTextures(const std::string& path, std::vector<ITexture*>& textures, const unsigned int& textureType = 0);
	Matrix4D AssimpMatrixToGLMMatrix(const aiMatrix4x4& mat);
	const bool WasMeshRead(const std::string& path);
	const bool WasTextureRead(const std::string& path);

	MeshReader();
	~MeshReader();
};

