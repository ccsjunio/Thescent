#pragma once
#include "IComponent.h"
#include <vector>
#include <MathLibrary.h>
#include "ModelDef.h"
#include <map>

class ISkinnedMesh : public IComponent
{
private:

protected:
	ISkinnedMesh() {};

public:
	virtual ~ISkinnedMesh() {};

	virtual void SetCurrentAnimation(const std::string& animationName) = 0;
	virtual const std::string& GetCurrentAnimation() = 0;
	virtual std::vector<Matrix4D> GetCurrentFinalTransformation() = 0;
	virtual std::vector<Matrix4D> GetCurrentOffsets() = 0;
	virtual std::vector<Matrix4D> GetCurrentObjectBoneTransformation() = 0;
	virtual const float GetDuration() = 0;
	virtual const float GetDuration(const std::string& animationName) = 0;
	virtual const bool PlayAnimation(const float& TimeInSeconds, const std::string& animationName) = 0;
	virtual const bool LoadMeshFromFile(const std::string& friendlyName, const std::string& filename) = 0;
	virtual const bool LoadMeshAnimation(const std::string& friendlyName, const std::string& filename) = 0;
	virtual const bool GetModelDrawInfo(ModelDrawInfo& mdi, unsigned int meshIndex = 0) = 0;
	virtual const bool GetAllBones(std::map<std::string, unsigned int>& boneMap) = 0;
	virtual const bool FindBoneByID(const unsigned int& id, std::string& boneName) = 0;
	virtual const bool FindBoneByName(const std::string& boneName, unsigned int& id) = 0;
};