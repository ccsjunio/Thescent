#pragma once
#include <IFactory.h>
#include <ITexture.h>
#include <IMesh.h>
#include <IParticleMeshComponent.h>
#include <functional>
#include "MeshReader.h"
#include <IDisposable.h>

class ParticleMeshComponentFactory : public IFactory<IParticleMeshComponent>, public IDisposable
{
private:
	MeshReader* meshReader;
	IParticleMeshComponent* CreateParticleMeshComponent(ParameterContainer& parameters);

public:
	std::function<const bool(ITexture*)> addTextureFunc;
	std::function<const bool(const std::string&, std::vector<ITexture*>&)> findTextureByNameFunc;
	std::function<const bool(IMesh*)> addMeshFunc;
	std::function<const bool(const std::string&, std::vector<IMesh*>&)> findMeshByPathFunc;

	ParticleMeshComponentFactory();
	~ParticleMeshComponentFactory();

	IParticleMeshComponent* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
	void Dispose();
};

