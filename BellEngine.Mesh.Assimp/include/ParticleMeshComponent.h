#pragma once
#include <IParticleMeshComponent.h>
#include <atomic>

class ParticleMeshComponent : public IParticleMeshComponent
{
private:	
	std::atomic<bool> isActive, allowMeshTransform;
	std::vector<IMesh*> meshes;
	std::vector<ITexture*> textures;
	std::map<IMesh*, std::vector<ITexture*>> meshTextureMap;
	std::string name;
	std::atomic<unsigned int> levelOfDetail;

public:
	ParticleMeshComponent();
	~ParticleMeshComponent();

	const bool AddTexture(ITexture* texture);
	const bool RemoveTexture(ITexture* texture);
	const bool GetTextures(std::vector<ITexture*>& textures);

	const bool AddTextureToMesh(IMesh* mesh, ITexture* texture);
	const bool RemoveTextureFromMesh(IMesh* mesh, ITexture* texture);
	const bool GetMeshTextures(IMesh* mesh, std::vector<ITexture*>& textures);

	void SetLevelOfDetail(const unsigned int& levelOfDetail);
	unsigned int GetLevelOfDetail();
	const bool AllowMeshTranformations();
	void ToggleMeshTransformations(const bool& value);

	const bool IsActive();
	void Activate(const bool& value = true);	
	const bool AddMesh(IMesh* mesh);
	const bool RemoveMesh(IMesh* mesh);
	const bool GetMeshes(std::vector<IMesh*>& meshes);
	void Dispose();
	const std::string GetName();
	void SetName(const std::string& name);
	const std::string GetType();
	const unsigned long long GetTypeID();
	ICloneable* Clone();
};
