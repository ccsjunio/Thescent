#pragma once
#include "ISystem.h"
#include "ParameterContainer.h"
#include <functional>

class ISystemDataPersistency : public ISystem
{
protected:
	ISystemDataPersistency() {};

public:
	struct OnFileLoad_s
	{
		std::string filePath, fileName, eventDescription;
		void* fileObject;
	};

	virtual ~ISystemDataPersistency() {};

	virtual void BindOnFileLoadCallback(std::function<void(const OnFileLoad_s&)> fn) = 0;
	virtual void* GetPointer(ParameterContainer& input, ParameterContainer& output) = 0;
};