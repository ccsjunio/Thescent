#pragma once
#include "ISystem.h"
#include <string>
#include <map>
#include <MathLibrary.h>
#include "ISound.h"
#include "Scene.h"
#include "IFactory.h"
#include "ISoundComponent.h"

class ISystemSound : public ISystem
{
protected:
	std::map<std::string, int> m_map_SoundIDByName;
	std::map<int, ISound*> m_map_pSoundsByID;
	std::string m_lastError;
	bool isOK = false;
	Scene* scene;

	ISystemSound() : scene(nullptr) {};

public:
	virtual ~ISystemSound() {};
	virtual void SetScene(Scene*& scene) = 0;
	virtual bool Initialize() = 0;
	virtual void Dispose() = 0;
	// Returns ID for sound, or 0 if there's an error
	virtual int Register(const std::string& fileName, const std::string& name, const int& type, const bool& overwriteIfPresent /*=false*/) = 0;
	virtual bool Play(const std::string& name, const bool& restartIfPlaying /*=true, which is default for OpenAL*/) = 0;
	virtual bool Delete(const std::string& name) = 0;
	// Returns zero if there's no sound
	virtual int FindSoundID(const std::string& name) = 0;
	// Returns nullptr or zero if there's no sound
	virtual ISound* FindSound(const std::string& name) = 0;
	virtual void LoopSound(const std::string& name, const bool& bLoop) = 0;
	virtual float GetPitch(const std::string& name) = 0;
	virtual void SetPitch(const std::string& name, const float& pitch) = 0;
	virtual float GetGain(const std::string& name) = 0;
	virtual void SetGain(const std::string& name, const float& gain) = 0;
	virtual Vector3D GetPosition(const std::string& name) = 0;
	virtual Vector3D GetVelocity(const std::string& name) = 0;
	virtual Vector3D GetDirection(const std::string& name) = 0;
	virtual void SetPosition(const std::string& name, const Vector3D& position) = 0;
	virtual void SetVelocity(const std::string& name, const Vector3D& velocity) = 0;
	virtual void SetDirection(const std::string& name, const Vector3D& direction) = 0;
	// Doppler and speed of sound
	virtual float GetDopplerFactor() = 0;
	virtual void SetDopplerFactor(const float& dopplerFactor) = 0;
	virtual void ResetDoppler() = 0;	// default is 1
	virtual float GetSpeedOfSound() = 0;
	virtual void SetSpeedOfSound(const float& speedOfSound) = 0;
	virtual void ResetSpeedOfSound() = 0;	// default is 343.3f
	virtual bool IsOK() = 0;
	virtual IFactory<ISoundComponent>* GetSoundComponentFactory() = 0;
};

