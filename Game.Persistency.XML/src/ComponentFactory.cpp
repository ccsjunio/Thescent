#include "ComponentFactory.h"
//#include <RenderComponent.h>
#include <StringHelpers.h>
#include <SkinnedMeshComponent.h>
#include <EngineFolders.h>

IComponent* ComponentFactory::CreateRenderComponent(ParameterContainer& parameters)
{
	/*Parameter temp;
	bool rc = false;

	RenderComponent* cp = new RenderComponent();

	rc = parameters.FindParameterByName("RenderMode", temp);
	if (rc) cp->SetRenderMode(atoi(temp.value.c_str()));
	rc = parameters.FindParameterByName("IsVisible", temp);
	if (rc) cp->SetVisibility(atoi(temp.value.c_str()) != 0);
	rc = parameters.FindParameterByName("Specular", temp);
	if (rc) cp->SetSpecular(StringHelpers::StrToVec4(temp.value));
	rc = parameters.FindParameterByName("Diffuse", temp);
	if (rc) cp->SetDiffuse(StringHelpers::StrToVec4(temp.value));

	return cp;*/

	return nullptr;
}

IComponent* ComponentFactory::CreateSkinnedMeshComponent(ParameterContainer& parameters)
{
	SkinnedMeshComponent* smc = new SkinnedMeshComponent();

	Parameter p;
	bool rc = parameters.FindParameterByName("Name", p);
	if (!rc) return nullptr;
	smc->SetName(p.value);

	rc = parameters.FindParameterByName("MeshName", p);
	if (!rc) return nullptr;
	std::string meshName = p.value;
	
	rc = parameters.FindParameterByName("MeshPath", p);
	if (!rc) return nullptr;
	std::string meshPath = EngineFolders::GetModelsPath() +  p.value;

	rc = smc->LoadMeshFromFile(meshName, meshPath);
	if (!rc) return nullptr;

	rc = parameters.FindParameterByName("AnimationCount", p);
	if (!rc) return nullptr;
	unsigned int animationCount = atol(p.value.c_str());
	if (animationCount == 0) return nullptr;

	std::string animationName, animationPath;
	for (unsigned int i = 0; i < animationCount; i++)
	{
		rc = parameters.FindParameterByName("AnimationName" + std::to_string(i), p);
		if (!rc) continue;
		animationName = p.value;

		rc = parameters.FindParameterByName("AnimationPath" + std::to_string(i), p);
		if (!rc) continue;
		animationPath = EngineFolders::GetModelsPath() + p.value;

		rc = smc->LoadMeshAnimation(animationName, animationPath);
	}

	rc = smc->Initialize();
	if (!rc)
	{
		delete smc;
		smc = nullptr;
	}

	return smc;
}

ComponentFactory::ComponentFactory()
{

}
ComponentFactory::~ComponentFactory()
{

}

IComponent* ComponentFactory::Create(ParameterContainer& parameters)
{
	IComponent* ptr = nullptr;

	Parameter type;
	bool rc = parameters.FindParameterByName("Type", type);

	if (rc)
	{
		if (type.value == "RenderComponent")
		{
			return CreateRenderComponent(parameters);
		}
		else if (type.value == "SkinnedMesh")
		{
			return CreateSkinnedMeshComponent(parameters);
		}
	}

	return ptr;
}
const bool ComponentFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return true;
}