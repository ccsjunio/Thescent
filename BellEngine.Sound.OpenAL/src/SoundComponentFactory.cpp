#include "SoundComponentFactory.h"
#include "SoundComponent.h"
#include <EngineFolders.h>
#include "Sound.h"

ISoundComponent* SoundComponentFactory::CreateSoundComponent(ParameterContainer& parameters)
{
	Parameter p;
	bool rc = false;

	rc = parameters.FindParameterByName("Name", p);
	if (!rc) return nullptr;
	std::string name = p.value;

	rc = parameters.FindParameterByName("Path", p);
	if (!rc) return nullptr;
	std::string path = EngineFolders::GetSoundsPath() + p.value;

	bool isActive = false;
	rc = parameters.FindParameterByName("IsActive", p);
	if (rc) isActive = atoi(p.value.c_str()) != 0;

	int sourceType = 0;
	if (parameters.FindParameterByName("SourceType", p))
	{
		sourceType = strtol(p.value.c_str(), NULL, 0);
		switch (sourceType)
		{
		case 1: sourceType = (int)Sound::sSourceType::TYPE_STATIC; break;
		case 2: sourceType = (int)Sound::sSourceType::TYPE_STREAMING; break;
		case 3: sourceType = (int)Sound::sSourceType::TYPE_UNDETERMINED; break;
		case 0:
		default:
			break;
		}
	}
	
	size_t flagCount = 0ll;
	rc = parameters.FindParameterByName("FlagCount", p);
	if (rc) flagCount = strtoul(p.value.c_str(), NULL, 0);

	size_t flags = 0ull;
	for (size_t i = 0; i < flagCount; i++)
	{
		if (parameters.FindParameterByName("Flag" + std::to_string(i), p))
		{
			flags |= strtoul(p.value.c_str(), NULL, 0);
		}
	}

	SoundComponent* sc = new SoundComponent();
	sc->SetName(name);
	sc->SetPath(path);
	sc->Activate(isActive);
	sc->SetSourceType(sourceType);
	sc->SetFlags(flags);

	//Sound::GetSourceType((ISound::Source_e)atoi(p.value.c_str()));

	if (registerSoundFunc)
	{
		registerSoundFunc(sc->GetPath(), sc->GetName(), sc->GetSourceType(), false);
	}

	return sc;
}

SoundComponentFactory::SoundComponentFactory()
{
}

SoundComponentFactory::~SoundComponentFactory()
{
}

ISoundComponent* SoundComponentFactory::Create(ParameterContainer& parameters)
{
	Parameter p;
	bool rc = false;

	if (parameters.FindParameterByName("Type", p))
	{
		if (p.value == "SoundComponent")
		{
			return CreateSoundComponent(parameters);
		}
	}

	return nullptr;
}

const bool SoundComponentFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return false;
}
