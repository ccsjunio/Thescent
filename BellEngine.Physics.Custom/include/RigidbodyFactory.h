#pragma once
#include <IFactory.h>
#include <IRigidbody.h>

class RigidbodyFactory : public IFactory<IRigidbody>
{
private:

protected:

public:
	RigidbodyFactory();
	~RigidbodyFactory();

	IRigidbody* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
};

