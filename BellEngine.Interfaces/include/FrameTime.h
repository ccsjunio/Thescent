#pragma once

struct FrameTime
{
private:
	double deltaTime, runTime;

public:
	FrameTime() : deltaTime(0.0), runTime(0.0) {};

	void SetDeltaTime(const double& deltaTime)
	{
		this->deltaTime = deltaTime;
	}
	const double GetDeltaTime()
	{
		return this->deltaTime;
	}

	void SetRunTime(const double& runTime)
	{
		this->runTime = runTime;
	}
	const double GetRunTime()
	{
		return this->runTime;
	}

	void Reset()
	{
		SetDeltaTime(0.0);
		SetRunTime(0.0);
	}
};