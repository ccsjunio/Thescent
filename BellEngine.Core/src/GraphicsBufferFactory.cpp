#include "GraphicsBufferFactory.h"
#include "FBO.h"
#include "StencilBuffer.h"

IGraphicsBuffer* GraphicsBufferFactory::CreateStencil(ParameterContainer& parameters)
{
	Parameter name;
	bool rc = parameters.FindParameterByName("Name", name);
	if (!rc) return nullptr;

	Parameter parent;
	rc = parameters.FindParameterByName("Parent", parent);
	if (!rc) return nullptr;

	Parameter target;
	rc = parameters.FindParameterByName("Target", target);
	if (!rc) return nullptr;

	Parameter passCount;
	rc = parameters.FindParameterByName("PassCount", passCount);
	if (!rc) return nullptr;

	int _passCount = atoi(passCount.value.c_str());
	if (_passCount <= 0) return nullptr;

	std::vector<int> passIDs;
	Parameter passID;
	for (int i = 0; i < _passCount; i++)
	{
		rc = parameters.FindParameterByName("Pass" + std::to_string(i), passID);
		if (rc)
		{
			passIDs.push_back(atoi(passID.value.c_str()));
		}
	}

	StencilBuffer* stencilBuffer = new StencilBuffer();
	stencilBuffer->SetName(name.value);
	stencilBuffer->SetParent(parent.value);
	stencilBuffer->SetTarget(target.value);
	for (size_t i = 0; i < passIDs.size(); i++)
	{
		rc = stencilBuffer->AddPassID(passIDs[i]);
	}
	return stencilBuffer;
}

IGraphicsBuffer* GraphicsBufferFactory::CreateFBO(ParameterContainer& parameters)
{
	Parameter name;
	bool rc = parameters.FindParameterByName("Name", name);
	if (!rc) return nullptr;

	Parameter parent;
	rc = parameters.FindParameterByName("Parent", parent);
	if (!rc) return nullptr;

	Parameter target;
	rc = parameters.FindParameterByName("Target", target);
	if (!rc) return nullptr;

	/*Camera is optional. If no camera is attached to the FBO, then it will use the camera of the Parent scene.*/
	Parameter cameraReference;
	rc = parameters.FindParameterByName("Camera", cameraReference);
	/*if (!rc) return nullptr;*/

	Parameter passCount;
	rc = parameters.FindParameterByName("PassCount", passCount);
	if (!rc) return nullptr;

	int _passCount = atoi(passCount.value.c_str());
	if (_passCount <= 0) return nullptr;

	std::vector<std::pair<int, bool>> passIDs;
	Parameter passID;
	for (int i = 0; i < _passCount; i++)
	{
		bool passState = true;
		int id = 0;
		if (parameters.FindParameterByName("Pass" + std::to_string(i), passID))
		{
			id  = atoi(passID.value.c_str());

			if (parameters.FindParameterByName("Pass" + std::to_string(i) + "State", passID)) passState = strtoul(passID.value.c_str(), NULL, 0) != 0;

			passIDs.push_back(std::pair<int, bool>(id, passState));
		}
	}

	FBO* fbo = new FBO();
	fbo->SetName(name.value);
	fbo->SetParent(parent.value);
	fbo->SetTarget(target.value);
	fbo->SetCameraReference(cameraReference.value);
	for (size_t i = 0; i < passIDs.size(); i++)
	{
		rc = fbo->AddPass(passIDs[i].first, passIDs[i].second);
	}
	return fbo;
}

GraphicsBufferFactory::GraphicsBufferFactory()
{

}
GraphicsBufferFactory::~GraphicsBufferFactory()
{

}

IGraphicsBuffer* GraphicsBufferFactory::Create(ParameterContainer& input)
{
	if (input.GetCount() > 0)
	{
		Parameter type;
		bool rc = input.FindParameterByName("Type", type);

		if (rc)
		{
			if (type.value == "fbo")
			{
				return CreateFBO(input);
			}
			else if (type.value == "stencil")
			{
				return CreateStencil(input);
			}
			else
			{
				return nullptr;
			}
		}
	}

	return nullptr;
}

const bool GraphicsBufferFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return true;
}
