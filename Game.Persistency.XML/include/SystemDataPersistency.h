#pragma once
#include "ISystemDataPersistency.h"
#include "LevelFactory.h"
#include <IShaderContainer.h>
#include <ShaderFileReader.h>

class SystemDataPersistency : public ISystemDataPersistency
{
private:
	bool isActive, isUpdateComplete;
	std::function<void(const OnFileLoad_s&)> onFileLoadCallback;
	LevelFactory* levelFactory;
	LevelFactory* GetLevelFactory();
	ShaderFileReader* shaderFileReader;

public:
	SystemDataPersistency();
	~SystemDataPersistency();

	const bool IsUpdateComplete();
	const bool IsActive();
	void Activate(const bool& value = true);
	void BindOnFileLoadCallback(std::function<void(const OnFileLoad_s&)> fn);
	void* GetPointer(ParameterContainer& input, ParameterContainer& output);
	void Update(const FrameTime& frameTime);
};

