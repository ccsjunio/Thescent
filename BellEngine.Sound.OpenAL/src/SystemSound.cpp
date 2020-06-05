#include "SystemSound.h"
#include <iostream>
#include <OpenAL/Framework.h>
#include <ISoundComponent.h>
#include "SoundComponentFactory.h"
#include "SoundComponentFlags.h"

SystemSound::SystemSound()
{
	this->isUpdateComplete = false;
	this->isActive = true;
	soundComponentFactory = new SoundComponentFactory();
	if (soundComponentFactory)
	{
		((SoundComponentFactory*)soundComponentFactory)->registerSoundFunc 
			= std::bind(&SystemSound::Register, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	}
}

SystemSound::~SystemSound()
{
	Dispose();
}

const bool SystemSound::StopSound(const std::string name)
{
	Sound* sound = FindSound(name);
	if (sound)
	{
		alSourceStop(sound->uiSource);
	}

	return false;
}

const bool SystemSound::IsSoundPlaying(const std::string name)
{
	Sound* sound = FindSound(name);
	if (sound)
	{
		alGetSourcei(sound->uiSource, AL_SOURCE_STATE, &(sound->iState));
		return sound->iState == AL_PLAYING;
	}

	return false;
}

void SystemSound::ReadFlags(ISoundComponent*& soundComponent)
{
	unsigned long long flags = soundComponent->GetFlags();

	if (flags & SC_LOOPED)
	{
		LoopSound(soundComponent->GetName(), true);
	}
}

const bool SystemSound::IsUpdateComplete()
{
	return this->isUpdateComplete.load();
}

const bool SystemSound::IsActive()
{
	return isActive.load();
}

void SystemSound::Activate(const bool& value)
{
	this->isActive.store(value);
}

IFactory<ISoundComponent>* SystemSound::GetSoundComponentFactory()
{
	return soundComponentFactory;
}

void SystemSound::SetScene(Scene*& scene)
{
	this->scene = scene;
}

bool SystemSound::Initialize()
{
	// Initialize Framework
	ALFWInit();

	//	ALFWprintf("PlayStatic Test Application\n");

	this->isOK = ALFWInitOpenAL();

	if (!isOK)
	{
		std::string error = "Failed to initialize OpenAL\n";
		SetLastErrorString(error);

		ALFWprintf(error.c_str());
		ALFWShutdown();
	}

	return isOK;
}

void SystemSound::Dispose()
{
	// Clean up by deleting Source(s) and Buffer(s)
	//	alSourceStop(uiSource);
	//  alDeleteSources(1, &uiSource);
	//	alDeleteBuffers(1, &uiBuffer);

	for (std::pair<int, ISound*> element : m_map_pSoundsByID)
	{
		Sound* pTheSound = (Sound*)element.second;
		if (pTheSound)
		{
			alSourceStop(pTheSound->uiSource);
			alDeleteSources(1, &(pTheSound->uiSource));
			alDeleteBuffers(1, &(pTheSound->uiBuffer));

			DisposePointer(pTheSound);
		}
	}

	// Clear the maps
	this->m_map_pSoundsByID.clear();
	this->m_map_SoundIDByName.clear();

	ALFWShutdownOpenAL();

	ALFWShutdown();

	this->isOK = false;

	DisposePointer(soundComponentFactory);
}

bool SystemSound::IsOK()
{
	return this->isOK;
}

void SystemSound::Update(const FrameTime& frameTime)
{
	if (!this->isActive.load()) return;
	this->isUpdateComplete.store(false);

	SetFrameTime(frameTime);

	if (scene)
	{
		std::vector<Entity*> entities;
		std::vector<ISoundComponent*> soundComponents;
		if (scene->GetEntities(entities))
		{
			for (const auto& entity : entities)
			{
				if (entity->IsDisposing()) continue;

				if (entity->GetComponents(soundComponents))
				{
					for (auto& sc : soundComponents)
					{
						std::string name = sc->GetName();

						auto flags = sc->GetFlags();
						/*If the sound is looped, don't low the activated flag*/
						if (flags & SC_LOOPED)
						{
							if (sc->IsActive())
							{
								if (!IsSoundPlaying(name))
								{
									ReadFlags(sc);
									Play(name, true);
								}
							}
							else
							{
								if (IsSoundPlaying(name))
								{
									StopSound(name);
								}
							}
						}
						/*It is a finite sound, we can deactivate it*/
						else
						{
							if (sc->IsActive())
							{
								ReadFlags(sc);

								Play(sc->GetName(), true);

								sc->Activate(false);
							}
						}
						/*if (sc->IsActive())
						{
							ReadFlags(sc);

							Play(sc->GetName(), true);

							sc->Activate(false);
						}*/
					}
				}
			}
		}
	}

	this->isUpdateComplete.store(true);
}

// Returns ID for sound, or 0 if there's an error
int SystemSound::Register(const std::string& soundPath, const std::string& name, const int& sourceType, const bool& overwriteIfPresent /*=false*/)
{
	int soundID = this->FindSoundID(name);

	if (soundID != 0)
	{	// There's a sound there. Are we to overwrite it?
		if (overwriteIfPresent)
		{	// Yes, so delete it
			this->Delete(soundID);
		}
		else
		{	// Don't overwrite, so return an error
			return -1;
		}//if ( overwriteIfPresent)
	}//if ( soundID != 0) 

	// At this point, there is no sound with that name, so add it
	
	Sound* sound = new Sound();
	sound->SetFileName(soundPath);
	sound->SetName(name);
	sound->iType = (ALint)sourceType;
	
	alGenBuffers((ALsizei)1, &(sound->uiBuffer));

	// Load Wave file into OpenAL Buffer
	//if (!ALFWLoadWaveToBuffer((char*)ALFWaddMediaPath(fileName.c_str()), uiBuffer))
	if (!ALFWLoadWaveToBuffer((char*)sound->GetFileName().c_str(), sound->uiBuffer))
	{	// Error
		alDeleteBuffers(1, &(sound->uiBuffer));
		delete (sound);

		return -1;
	}//if (!ALFWLoadWaveToBuffer(...

	// Generate a Source to playback the buffer
	alGenSources(1, &(sound->uiSource));

	// Attach Source to buffer
	alSourcei(sound->uiSource, AL_BUFFER, sound->uiBuffer);

	// Add the sound to the maps
	this->m_map_pSoundsByID[sound->GetUniqueID()] = sound;
	this->m_map_SoundIDByName[sound->GetName()] = sound->GetUniqueID();

	return sound->GetUniqueID();
}

bool SystemSound::Play(const std::string& name, const bool& restartIfPlaying /*=true, which is default for OpenAL*/)
{
	// Find sound in map
	int soundID = this->FindSoundID(name);
	if (soundID == 0)
	{	// No sound of that name
		return false;	// Error;
	}

	return this->Play(soundID, restartIfPlaying);
}

bool SystemSound::Play(const int& soundUniqueID, const bool& restartIfPlaying /*=true, which is default for OpenAL*/)
{
	Sound* pSound = this->FindSound(soundUniqueID);
	if (pSound == nullptr)
	{	// There's no sound with that ID
		return false;	// Error
	}


	// OpenAL default is to restart the sound if it's already playing
	// So if we AREN'T doing this, then check state of sound
	if (!restartIfPlaying)
	{
		alGetSourcei(pSound->uiSource, AL_SOURCE_STATE, &(pSound->iState));
		if (pSound->iState == AL_PLAYING)
		{
			return true;	// Is already playing
		}
	}// if ( ! restartIfPlaying )

	// Either the sound isn't currently playing, or we want it restarted
	alSourcePlay(pSound->uiSource);

	return true;
}

bool SystemSound::Delete(const std::string& name)
{
	int soundID = this->FindSoundID(name);
	if (soundID == 0)
	{	// Didn't find it
		return false;	// Error
	}

	return this->Delete(soundID);
}

bool SystemSound::Delete(const int& soundUniqueID)
{
	Sound* pSound = this->FindSound(soundUniqueID);
	if (pSound == nullptr)
	{	// Didn't find it
		return false;	// Error
	}

	// Delete the things from the maps
	this->m_map_SoundIDByName.erase(pSound->GetName());
	this->m_map_pSoundsByID.erase(pSound->GetUniqueID());

	// Clean up by deleting Source(s) and Buffer(s)
	alSourceStop(pSound->uiSource);
	// In the example code, these happen immediately, so I assume we
	//	don't have to spin, waiting for the sound to stop or anything
	alDeleteSources(1, &(pSound->uiSource));
	alDeleteBuffers(1, &(pSound->uiBuffer));

	delete pSound;

	return true;
}

void SystemSound::LoopSound(const std::string& name, const bool& bLoop)
{
	int soundID = this->FindSoundID(name);
	if (soundID != 0)
	{
		this->LoopSound(soundID, bLoop);
	}
	return;
}

void SystemSound::LoopSound(const int& soundID, const bool& bLoop)
{
	Sound* pTheSound = this->FindSound(soundID);
	if (pTheSound != nullptr)
	{
		alSourcei(pTheSound->uiSource, AL_LOOPING, (bLoop ? AL_TRUE : AL_FALSE));
	}
	return;
}

float SystemSound::GetPitch(const std::string& name)
{
	int soundID = this->FindSoundID(name);
	if (soundID != 0)
	{
		return this->GetPitch(soundID);
	}
	return 0.0f;	// Error condition (or the sound is actually at 0.0f pitch, I suppose
}

float SystemSound::GetPitch(const int& soundID)
{
	float pitch = 0.0f;
	Sound* pTheSound = this->FindSound(soundID);
	if (pTheSound != nullptr)
	{
		alGetSourcef(pTheSound->uiSource, AL_PITCH, &pitch);
	}
	return pitch;
}

void SystemSound::SetPitch(const std::string& name, const float& pitch)
{
	int soundID = this->FindSoundID(name);

	if (soundID != 0)
	{
		this->SetPitch(soundID, pitch);
	}
}

void SystemSound::SetPitch(const int& soundID, const float& pitch)
{
	Sound* pTheSound = this->FindSound(soundID);
	if (pTheSound != nullptr)
	{
		alSourcef(pTheSound->uiSource, AL_PITCH, pitch);
	}
	return;
}

float SystemSound::GetGain(const std::string& name)
{
	int soundID = this->FindSoundID(name);
	if (soundID != 0)
	{
		return this->GetGain(soundID);
	}
	return 0.0f;	// Error condition (or the sound is actually at 0.0f gain, I suppose
}

float SystemSound::GetGain(const int& soundID)
{
	float gain = 0.0f;
	Sound* pTheSound = this->FindSound(soundID);
	if (pTheSound != nullptr)
	{
		alGetSourcef(pTheSound->uiSource, AL_GAIN, &gain);
	}
	return gain;
}

void SystemSound::SetGain(const std::string& name, const float& gain)
{
	int soundID = this->FindSoundID(name);
	if (soundID != 0)
	{
		this->SetGain(soundID, gain);
	}
	return;
}

void SystemSound::SetGain(const int& soundID, const float& gain)
{
	Sound* pTheSound = this->FindSound(soundID);
	if (pTheSound != nullptr)
	{
		alSourcef(pTheSound->uiSource, AL_GAIN, gain);
	}
	return;
}

Vector3D SystemSound::GetPosition(const std::string& name)
{
	int soundID = this->FindSoundID(name);
	if (soundID != 0)
	{
		return this->GetPosition(soundID);
	}
	return Vector3D(0.0f);
}

Vector3D SystemSound::GetPosition(const int& soundID)
{
	Vector3D position(0.0f);
	Sound* pTheSound = this->FindSound(soundID);
	if (pTheSound != nullptr)
	{
		alGetSource3f(pTheSound->uiSource, AL_POSITION, &(position.x), &(position.y), &(position.z));
	}
	return position;
}

Vector3D SystemSound::GetVelocity(const std::string& name)
{
	int soundID = this->FindSoundID(name);
	if (soundID != 0)
	{
		return this->GetVelocity(soundID);
	}
	return Vector3D(0.0f);
}

Vector3D SystemSound::GetVelocity(const int& soundID)
{
	Vector3D velocity(0.0f);
	Sound* pTheSound = this->FindSound(soundID);
	if (pTheSound != nullptr)
	{
		alGetSource3f(pTheSound->uiSource, AL_VELOCITY, &(velocity.x), &(velocity.y), &(velocity.z));
	}
	return velocity;
}

Vector3D SystemSound::GetDirection(const std::string& name)
{
	int soundID = this->FindSoundID(name);
	if (soundID != 0)
	{
		return this->GetDirection(soundID);
	}
	return Vector3D(0.0f);
}

Vector3D SystemSound::GetDirection(const int& soundID)
{
	Vector3D direction(0.0f);
	Sound* pTheSound = this->FindSound(soundID);
	if (pTheSound != nullptr)
	{
		alGetSource3f(pTheSound->uiSource, AL_DIRECTION, &(direction.x), &(direction.y), &(direction.z));
	}
	return direction;
}

void SystemSound::SetPosition(const std::string& name, const Vector3D& position)
{
	int soundID = this->FindSoundID(name);
	if (soundID != 0)
	{
		this->SetPosition(soundID, position);
	}
	return;
}

void SystemSound::SetPosition(const int& soundID, const Vector3D& position)
{
	Sound* pTheSound = this->FindSound(soundID);
	if (pTheSound != nullptr)
	{
		alSource3f(pTheSound->uiSource, AL_POSITION, position.x, position.y, position.z);
	}
}

void SystemSound::SetVelocity(const std::string& name, const Vector3D& velocity)
{
	int soundID = this->FindSoundID(name);
	if (soundID != 0)
	{
		this->SetVelocity(soundID, velocity);
	}
	return;
}

void SystemSound::SetVelocity(const int& soundID, const Vector3D& velocity)
{
	Sound* pTheSound = this->FindSound(soundID);
	if (pTheSound != nullptr)
	{
		alSource3f(pTheSound->uiSource, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	}
	return;
}

void SystemSound::SetDirection(const std::string& name, const Vector3D& direction)
{
	int soundID = this->FindSoundID(name);
	if (soundID != 0)
	{
		this->SetDirection(soundID, direction);
	}
	return;
}

void SystemSound::SetDirection(const int& soundID, const Vector3D& direction)
{
	Sound* pTheSound = this->FindSound(soundID);
	if (pTheSound != nullptr)
	{
		alSource3f(pTheSound->uiSource, AL_DIRECTION, direction.x, direction.y, direction.z);
	}
	return;
}

// Returns zero if there's no sound
int SystemSound::FindSoundID(const std::string& name)
{
	std::map<std::string, int>::iterator itSound = this->m_map_SoundIDByName.find(name);

	if (itSound == this->m_map_SoundIDByName.end())
	{	// Nope, didn't find it
		return 0;
	}

	// Found ID
	return itSound->second;
}

float SystemSound::GetDopplerFactor()
{
	return alGetFloat(AL_DOPPLER_FACTOR);
}

void SystemSound::SetDopplerFactor(const float& dopplerFactor)
{
	alDopplerFactor(dopplerFactor);
	return;
}

void SystemSound::ResetDoppler()
{	// default is 1.0f
	this->SetDopplerFactor(1.0f);
	return;
}

float SystemSound::GetSpeedOfSound()
{
	return alGetFloat(AL_SPEED_OF_SOUND);
}

void SystemSound::SetSpeedOfSound(const float& speedOfSound)
{
	alSpeedOfSound(speedOfSound);
	return;
}

void SystemSound::ResetSpeedOfSound()
{	// default is 343.3f
	this->SetSpeedOfSound(343.3f);
	return;
}

// Returns nullptr or zero if there's no sound
Sound* SystemSound::FindSound(const std::string& name)
{
	int soundID = this->FindSoundID(name);
	if (soundID == 0)
	{	// Didn't find it
		return nullptr;
	}

	std::map<int, ISound*>::iterator itSound = this->m_map_pSoundsByID.find(soundID);
	if (itSound == this->m_map_pSoundsByID.end())
	{	// Didn't find it
		return nullptr;
	}
	// Found it
	return (Sound*)itSound->second;
}

Sound* SystemSound::FindSound(const int& soundUniqueID)
{
	std::map<int, ISound*>::iterator itSound = this->m_map_pSoundsByID.find(soundUniqueID);

	if (itSound == this->m_map_pSoundsByID.end())
	{	// Didn't find it
		return nullptr;
	}

	// Found it
	return (Sound*)itSound->second;
}