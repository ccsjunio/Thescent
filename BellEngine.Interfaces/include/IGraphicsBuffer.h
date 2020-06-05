#pragma once
#include "IIdentifiable.h"

class IGraphicsBuffer : public IIdentifiable
{
protected:
	IGraphicsBuffer() {};

public:
	virtual ~IGraphicsBuffer() {};
};