#pragma once
#include <IFactory.h>
#include <ISoftbody.h>
#include <ICollider.h>
#include <IDisposable.h>

class SoftbodyFactory : public IFactory<ISoftbody>, public IDisposable
{
private:
	IFactory<ICollider>* colliderFactory;

	ISoftbody* CreateSoftbody(ParameterContainer& parameters);
	ISoftbody* CreateCloth(ParameterContainer& parameters);

public:
	SoftbodyFactory();
	~SoftbodyFactory();

	ISoftbody* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
	void Dispose();
};

