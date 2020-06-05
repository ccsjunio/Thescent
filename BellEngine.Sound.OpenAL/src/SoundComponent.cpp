#include "SoundComponent.h"
#include <BellEngineID.h>

SoundComponent::SoundComponent()
{
	this->sourceType = 0;
	this->isActive = false;
}

SoundComponent::~SoundComponent()
{
}

void SoundComponent::SetFlags(const unsigned long long& flags)
{
	this->flags.store(flags);
}

const unsigned long long SoundComponent::GetFlags()
{
	return this->flags.load();
}

const int SoundComponent::GetSourceType()
{
	return this->sourceType.load();
}

void SoundComponent::SetSourceType(const int& type)
{
	this->sourceType.store(type);
}

const std::string SoundComponent::GetName()
{
	return this->name;
}

void SoundComponent::SetName(const std::string& name)
{
	this->name = name;
}

void SoundComponent::SetPath(const std::string& path)
{
	this->path = path;
}

const std::string SoundComponent::GetPath()
{
	return path;
}

const bool SoundComponent::IsActive()
{
	return isActive.load();
}

void SoundComponent::Activate(const bool& value)
{
	this->isActive.store(value);
}

ICloneable* SoundComponent::Clone()
{
	SoundComponent* clone = new SoundComponent();
	clone->SetName(GetName());
	clone->SetPath(GetPath());
	clone->Activate(IsActive());

	return clone;
}

void SoundComponent::Dispose()
{
}

const std::string SoundComponent::GetType()
{
	return "SoundComponent";
}

const unsigned long long SoundComponent::GetTypeID()
{
	return ENGINE_OBJ_ID_SND_SOUNDCOMPONENT;
}
