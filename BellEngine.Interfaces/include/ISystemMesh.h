#pragma once
#include <string>
#include <map>
#include <ISystem.h>
#include <IMesh.h>
#include <IMeshComponent.h>
#include <IParticleMeshComponent.h>
#include <IFactory.h>

class ISystemMesh : public ISystem
{
protected:	
	ISystemMesh() {};

public:
	virtual ~ISystemMesh() {};

	virtual const bool GetMeshes(std::map<std::string, IMesh*>& meshMap) = 0;
	virtual IFactory<IMeshComponent>* GetMeshComponentFactory() = 0;
	virtual IFactory<IParticleMeshComponent>* GetParticleMeshComponentFactory() = 0;
	virtual const bool RegisterMesh(IMesh* mesh) = 0;
	virtual const bool Remove(const std::string& path) = 0;
	virtual ITexture* FindTextureByName(const std::string& name) = 0;
	virtual IMesh* FindMeshByName(const std::string& name) = 0;
	virtual const bool FindMeshByPath(const std::string& path, std::vector<IMesh*>& meshes) = 0;	
};

