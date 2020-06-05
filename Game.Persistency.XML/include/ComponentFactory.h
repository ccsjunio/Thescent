#pragma once
#include <IFactory.h>
#include <IComponent.h>

class ComponentFactory : public IFactory<IComponent>
{
private:
	IComponent* CreateRenderComponent(ParameterContainer& parameters);
	IComponent* CreateSkinnedMeshComponent(ParameterContainer& parameters);

public:
	ComponentFactory();
	~ComponentFactory();

	IComponent* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
};