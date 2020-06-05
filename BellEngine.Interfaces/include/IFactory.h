#pragma once
#include "IExtendedInfo.h"
#include "ParameterContainer.h"

template <typename T>
class IFactory : public IExtendedInfo
{
protected:
	IFactory() {};

public:
	virtual ~IFactory() {};

	virtual T* Create(ParameterContainer& parameters) = 0;
	virtual const bool Initialize(ParameterContainer& input, ParameterContainer& output) = 0;
};