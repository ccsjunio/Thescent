#pragma once
#include <ISystemSound.h>
#include <string>
#include <map>
#include <Sound.h>
#include <IDisposable.h>
#include <atomic>

class SystemSound : public ISystemSound, public IDisposable
{
private:
	std::atomic<bool> isActive, isUpdateComplete;
	IFactory<ISoundComponent>* soundComponentFactory;	

public:
	SystemSound();
	~SystemSound();

	const bool StopSound(const std::string name);
	const bool IsSoundPlaying(const std::string name);
	void ReadFlags(ISoundComponent*& soundComponent);
	const bool IsUpdateComplete();
	const bool IsActive();
	void Activate(const bool& value = true);
	IFactory<ISoundComponent>* GetSoundComponentFactory();
	void SetScene(Scene*& scene);
	bool Initialize();
	void Dispose();
	// Returns ID for sound, or 0 if there's an error
	int Register(const std::string& fileName, const std::string& name, const int& type, const bool& overwriteIfPresent /*=false*/);
	bool Play(const std::string& name, const bool& restartIfPlaying /*=true, which is default for OpenAL*/);
	bool Play(const int& soundUniqueID, const bool& restartIfPlaying /*=true, which is default for OpenAL*/);
	bool Delete(const std::string& name);
	bool Delete(const int& soundUniqueID);
	// Returns zero if there's no sound
	int FindSoundID(const std::string& name);
	// Returns nullptr or zero if there's no sound
	Sound* FindSound(const std::string& name);
	Sound* FindSound(const int& soundUniqueID);
	void LoopSound(const std::string& name, const bool& bLoop);
	void LoopSound(const int& soundID, const bool& bLoop);
	float GetPitch(const std::string& name);
	float GetPitch(const int& soundID);
	void SetPitch(const std::string& name, const float& pitch);
	void SetPitch(const int& soundID, float const& pitch);
	float GetGain(const std::string& name);
	float GetGain(const int& soundID);
	void SetGain(const std::string& name, const float& gain);
	void SetGain(const int& soundID, float const& gain);
	Vector3D GetPosition(const std::string& name);
	Vector3D GetPosition(const int& soundID);
	Vector3D GetVelocity(const std::string& name);
	Vector3D GetVelocity(const int& soundID);
	Vector3D GetDirection(const std::string& name);
	Vector3D GetDirection(const int& soundID);
	void SetPosition(const std::string& name, const Vector3D& position);
	void SetPosition(const int& soundID, const Vector3D& position);
	void SetVelocity(const std::string& name, const Vector3D& velocity);
	void SetVelocity(const int& soundID, const Vector3D& velocity);
	void SetDirection(const std::string& name, const Vector3D& direction);
	void SetDirection(const int& soundID, const Vector3D& direction);
	// Doppler and speed of sound
	float GetDopplerFactor();
	void SetDopplerFactor(float const& dopplerFactor);
	void ResetDoppler();	// default is 1
	float GetSpeedOfSound();
	void SetSpeedOfSound(float const& speedOfSound);
	void ResetSpeedOfSound();	// default is 343.3f
	bool IsOK();
	void Update(const FrameTime& deltaTime);
};

