#pragma once
#include "FrameTime.h"

class IUpdatable
{
protected:
	FrameTime frameTime;

	IUpdatable() : frameTime() {};

public:
	virtual ~IUpdatable() {};

	virtual const bool IsUpdateComplete() = 0;

	void SetFrameTime(const FrameTime& frameTime)
	{
		this->frameTime = frameTime;
	}
	const FrameTime GetFrameTime()
	{
		return this->frameTime;
	}

	virtual void Update(const FrameTime& frameTime) = 0;
};