#pragma once
#include <ISoundComponent.h>
#include <atomic>

class SoundComponent : public ISoundComponent
{
private:
	std::string path;
	std::atomic<bool> isActive;
	std::string name;
	std::atomic<int> sourceType;
	std::atomic<unsigned long long> flags;

public:
	SoundComponent();
	~SoundComponent();

	void SetFlags(const unsigned long long& flags);
	const unsigned long long GetFlags();
	const int GetSourceType();
	void SetSourceType(const int& type);
	const std::string GetName();
	void SetName(const std::string& name);
	void SetPath(const std::string& path);
	const std::string GetPath();
	const bool IsActive();
	void Activate(const bool& option = true);
	ICloneable* Clone();
	void Dispose();
	const std::string GetType();
	const unsigned long long GetTypeID();
};

