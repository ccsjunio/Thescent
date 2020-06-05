#pragma once
#include <string>
#include <map>
#include <ISound.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <OpenAL/efx.h>
#include <OpenAL/efx-creative.h>
#include <OpenAL/EFX-Util.h>

class Sound : public ISound
{
private:
	std::string name;

public:
	enum class sSourceState	// AL_SOURCE_STATE
	{	// These are straight duplicates of the OpenAL source states
		STATE_INITIAL = 0x1011,	// #define AL_INITIAL 0x1011
		STATE_PLAYING = 0x1012,
		STATE_PAUSED = 0x1013,
		STATE_STOPPED = 0x1014,
		STATE_UNKNOWN_OR_INVALID = 0
	};
	enum class sSourceType	// AL_SOURCE_TYPE
	{	// These are straight duplicates of the OpenAL source types
		TYPE_STATIC = 0x1028,	// #define AL_STATIC 0x1028
		TYPE_STREAMING = 0x1029,
		TYPE_UNDETERMINED = 0x1030,
		TYPE_UNKNOWN_OR_INVALID = 0
	};
	ALuint      uiBuffer;
	ALuint      uiSource;
	ALint       iState;		// AL_SOURCE_STATE: AL_INITIAL, AL_PLAYING, AL_PAUSED, AL_STOPPED
	ALint		iType;		// AL_SOURCE_TYPE: AL_STATIC, AL_STREAMING, AL_UNDETERMINED

	Sound();

	static Sound::sSourceType GetSourceType(ISound::Source_e source);
	const std::string GetName();
	void SetName(const std::string& name);
	std::string GetStateString();
	std::string GetTypeString();
	int GetSourceState();
	int GetSourceType();
	int GetUniqueID();
	const std::string GetType();
	const unsigned long long GetTypeID();
};

