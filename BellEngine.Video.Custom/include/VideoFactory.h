#pragma once
#include <IFactory.h>
#include <IComponent.h>

class VideoFactory : public IFactory<IComponent>
{
private:
	IComponent* CreateRenderComponent(ParameterContainer& parameters);
	IComponent* CreateClothRenderComponent(ParameterContainer& parameters);
	IComponent* CreateParticleEmitterComponent(ParameterContainer& parameters);
	IComponent* CreateDirectionalLightComponent(ParameterContainer& parameters);
	IComponent* CreateSpotLightComponent(ParameterContainer& parameters);
	IComponent* CreatePointLightComponent(ParameterContainer& parameters);

public:
	VideoFactory();
	~VideoFactory();

	IComponent* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
};

