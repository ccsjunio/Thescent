#pragma once
#include <IMesh.h>
#include <ITexture.h>
#include <IExtendedInfo.h>

class MeshReader : public IExtendedInfo
{
public:
	MeshReader();
	~MeshReader();

	const bool ReadMesh(const std::string& path, std::vector<IMesh*>& meshes);
	const bool ReadTextures(const std::string& path, std::vector<ITexture*>& textures);
};

