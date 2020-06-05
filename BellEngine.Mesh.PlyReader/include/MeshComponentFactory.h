#pragma once
#include <IFactory.h>
#include <IMeshComponent.h>
#include <MeshReader.h>
#include <IDisposable.h>

class MeshComponentFactory : public IFactory<IMeshComponent>, public IDisposable
{
private:
	MeshReader* meshReader;

public:
	MeshComponentFactory();
	~MeshComponentFactory();

	IMeshComponent* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
	void Dispose();
};

