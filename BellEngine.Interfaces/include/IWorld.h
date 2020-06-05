#pragma once
#include "IExtendedInfo.h"
#include "ISystem.h"
#include "FrameTime.h"
#include "IUpdatable.h"

class IWorld : public IUpdatable, public IExtendedInfo
{
private:
	

protected:
	IWorld() {};

public:
	virtual ~IWorld() {};

	virtual ISystem* GetSystem(const std::string& systemName) = 0;
	virtual void Update(const FrameTime& deltaTime) = 0;
	virtual void* GetPointer(const std::string& name) = 0;
};