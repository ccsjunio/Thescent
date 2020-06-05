#pragma once
#include "IMeshComponent.h"
#include "IMesh.h"
#include <vector>

class IParticleMeshComponent : public IMeshComponent
{
protected:
	IParticleMeshComponent() {};

public:
	virtual ~IParticleMeshComponent() {};
};