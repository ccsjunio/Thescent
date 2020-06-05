#pragma once
#include <IFactory.h>
#include <IMeshComponent.h>
#include <MeshReader.h>
#include <IDisposable.h>
#include <map>
#include <vector>
#include <functional>

class MeshComponentFactory : public IFactory<IMeshComponent>, public IDisposable
{
private:
	MeshReader* meshReader;	
	std::map<std::string, IMesh*>* meshMap;
	std::vector<ITexture*>* textures;

	IMeshComponent* CreateMeshComponent(ParameterContainer& parameters);

public:
	MeshComponentFactory();
	~MeshComponentFactory();

	IMeshComponent* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
	void Dispose();
};

