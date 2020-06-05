#include "Sound.h"
#include <BellEngineID.h>

int Sound::m_NextUniqueID = 1000;	// Sounds start at 1000 (0 is an error)

Sound::Sound()
{
	uiBuffer = 0;	// Unknown
	uiSource = 0;	// Unknown
	iState = AL_INITIAL;
	iType = AL_UNDETERMINED;
	this->m_UniqueID = Sound::m_NextUniqueID;
	Sound::m_NextUniqueID++;
}

Sound::sSourceType Sound::GetSourceType(ISound::Source_e source)
{
	switch (source)
	{
	case ISound::Source_e::Sampled:
		return Sound::sSourceType::TYPE_STATIC;
		break;
	case ISound::Source_e::Streamed:
		return Sound::sSourceType::TYPE_STREAMING;
		break;
	default:
		break;
	}

	return Sound::sSourceType::TYPE_STATIC;
}

const std::string Sound::GetName()
{
	return this->name;
}

void Sound::SetName(const std::string& name)
{
	this->name = name;
}

int Sound::GetUniqueID()
{
	return this->m_UniqueID;
}

int Sound::GetSourceState()
{
	switch (this->iState)
	{
	case AL_INITIAL:
		return (int)Sound::sSourceState::STATE_INITIAL;
	case AL_PLAYING:
		return (int)Sound::sSourceState::STATE_PLAYING;
	case AL_PAUSED:
		return (int)Sound::sSourceState::STATE_PAUSED;
	case AL_STOPPED:
		return (int)Sound::sSourceState::STATE_STOPPED;
	default:
		return (int)Sound::sSourceState::STATE_UNKNOWN_OR_INVALID;
	}
}

std::string Sound::GetStateString()
{
	switch (this->iState)
	{
	case AL_INITIAL:
		return "STATE_INITIAL";
	case AL_PLAYING:
		return "STATE_PLAYING";
	case AL_PAUSED:
		return "STATE_PAUSED";
	case AL_STOPPED:
		return "STATE_STOPPED";
	default:
		return "STATE_UNKNOWN_OR_INVALID";
	}
}

int Sound::GetSourceType()
{
	// AL_SOURCE_TYPE
	switch (this->iType)
	{
	case AL_STATIC:
		return (int)Sound::sSourceType::TYPE_STATIC;
	case AL_STREAMING:
		return (int)Sound::sSourceType::TYPE_STREAMING;
	case AL_UNDETERMINED:
		return (int)Sound::sSourceType::TYPE_UNDETERMINED;
	default:
		return (int)Sound::sSourceType::TYPE_UNDETERMINED;
	}
}

std::string Sound::GetTypeString()
{
	switch (this->iType)
	{
	case AL_STATIC:
		return "TYPE_STATIC";
	case AL_STREAMING:
		return "TYPE_STREAMING";
	case AL_UNDETERMINED:
		return "TYPE_UNDETERMINED";
	default:
		return "TYPE_UNDETERMINED";
	}
}

const std::string Sound::GetType()
{
	return "Sound";
}

const unsigned long long Sound::GetTypeID()
{
	return ENGINE_OBJ_ID_SND_SOUND;
}
