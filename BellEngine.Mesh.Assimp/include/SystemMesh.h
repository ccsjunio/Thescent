#pragma once
#include <string>
#include <map>
#include <ISystemMesh.h>
#include <assimp\matrix4x4.h>
#include <IDisposable.h>

class SystemMesh : public ISystemMesh, public IDisposable
{
private:
	IFactory<IMeshComponent>* meshComponentFactory;
	IFactory<IParticleMeshComponent>* particleMeshComponentFactory;
	bool isActive, isUpdateComplete;
	std::vector<ITexture*> textures;
	std::map<std::string, IMesh*> meshMap;
	Matrix4D AssimpMatrixToGLMMatrix(const aiMatrix4x4& mat);

public:
	SystemMesh();
	~SystemMesh();

	const bool AddTexture(ITexture* texture);
	const bool FindTexturesByName(const std::string& name, std::vector<ITexture*>& tx);
	const bool AddMesh(IMesh* mesh);
	const bool FindMeshesByPath(const std::string& path, std::vector<IMesh*>& ms);

	const bool IsUpdateComplete();
	const bool GetMeshes(std::map<std::string, IMesh*>& meshMap);
	const bool IsActive();
	void Activate(const bool& value = true);
	IFactory<IMeshComponent>* GetMeshComponentFactory();
	IFactory<IParticleMeshComponent>* GetParticleMeshComponentFactory();
	const bool RegisterMesh(IMesh* mesh);
	const bool Remove(const std::string& path);
	ITexture* FindTextureByName(const std::string& path);
	IMesh* FindMeshByName(const std::string& name);
	const bool FindMeshByPath(const std::string& path, std::vector<IMesh*>& meshes);
	void Update(const FrameTime& deltaTime);
	void Dispose();
};

