#pragma once
#include <IFactory.h>
#include <ISoundComponent.h>
#include <functional>

class SoundComponentFactory : public IFactory<ISoundComponent>
{
private:
	ISoundComponent* CreateSoundComponent(ParameterContainer& parameters);

public:	
	std::function<int(const std::string&, const std::string&, const int& sourceType, const bool& overwriteIfPresent)> registerSoundFunc;

	SoundComponentFactory();
	~SoundComponentFactory();

	ISoundComponent* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
};

