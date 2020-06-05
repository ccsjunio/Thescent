#pragma once
#include <IMeshComponent.h>

class MeshComponent : public IMeshComponent
{
private:
	unsigned int levelOfDetail;
	bool isActive;
	bool allowMeshTransform;
	std::vector<ITexture*> textures;
	std::vector<IMesh*> meshes;
	std::map<IMesh*, std::vector<ITexture*>> meshTextureMap;
	std::string name;

public:
	MeshComponent();
	~MeshComponent();

	const bool AddMesh(IMesh* mesh);
	const bool RemoveMesh(IMesh* mesh);
	const bool GetMeshes(std::vector<IMesh*>& meshes);

	const bool AddTexture(ITexture* texture);
	const bool RemoveTexture(ITexture* texture);
	const bool GetTextures(std::vector<ITexture*>& textures);

	const bool AddTextureToMesh(IMesh* mesh, ITexture* texture);
	const bool RemoveTextureFromMesh(IMesh* mesh, ITexture* texture);
	const bool GetMeshTextures(IMesh* mesh, std::vector<ITexture*>& textures);

	void SetLevelOfDetail(const unsigned int& levelOfDetail);
	unsigned int GetLevelOfDetail();
	const bool IsActive();
	void Activate(const bool& value = true);
	const bool AllowMeshTranformations();
	void ToggleMeshTransformations(const bool& value);
	
	void Dispose();
	const std::string GetName();
	void SetName(const std::string& name);
	const std::string GetType();
	const unsigned long long GetTypeID();
	ICloneable* Clone();
};

