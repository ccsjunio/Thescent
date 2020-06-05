#pragma once
#include <IFactory.h>
#include <ISoftbody.h>

class SoftbodyFactory : public IFactory<ISoftbody>
{

public:
	SoftbodyFactory() {};
	~SoftbodyFactory() {};

	ISoftbody* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
};

