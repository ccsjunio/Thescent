#pragma once
#include <IFactory.h>
#include <IGraphicsBuffer.h>

class GraphicsBufferFactory : public IFactory<IGraphicsBuffer>
{
private:
	IGraphicsBuffer* CreateStencil(ParameterContainer& parameters);
	IGraphicsBuffer* CreateFBO(ParameterContainer& parameters);

public:
	GraphicsBufferFactory();
	~GraphicsBufferFactory();

	IGraphicsBuffer* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
};

