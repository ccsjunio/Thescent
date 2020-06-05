#pragma once
#include <ISystemMesh.h>
#include <IDisposable.h>

class SystemMesh : public ISystemMesh, public IDisposable
{
private:
	IFactory<IMeshComponent>* meshComponentFactory;
	bool isActive, isUpdateComplete;
	std::vector<ITexture*> textures;
	std::map<std::string, IMesh*> meshMap;

public:
	SystemMesh();
	~SystemMesh();

	IFactory<IParticleMeshComponent>* GetParticleMeshComponentFactory();
	const bool IsUpdateComplete();
	const bool GetMeshes(std::map<std::string, IMesh*>& meshMap);
	const bool IsActive();
	void Activate(const bool& value = true);
	IFactory<IMeshComponent>* GetMeshComponentFactory();
	const bool RegisterMesh(IMesh* mesh);
	const bool Remove(const std::string& path);
	ITexture* FindTextureByName(const std::string& path);
	IMesh* FindMeshByName(const std::string& name);
	const bool FindMeshByPath(const std::string& path, std::vector<IMesh*>& meshes);
	void Update(const FrameTime& deltaTime);
	void Dispose();
};

