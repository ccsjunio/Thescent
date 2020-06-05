#pragma once
#include "IComponent.h"
#include <vector>
#include "IMesh.h"
#include "ITexture.h"

class IMeshComponent : public IComponent
{
protected:
	IMeshComponent() {};

public:
	virtual ~IMeshComponent() {};

	virtual void SetLevelOfDetail(const unsigned int& levelOfDetail) = 0;
	virtual unsigned int GetLevelOfDetail() = 0;
	virtual const bool AllowMeshTranformations() = 0;
	virtual void ToggleMeshTransformations(const bool& value) = 0;

	virtual const bool AddMesh(IMesh* mesh) = 0;
	virtual const bool RemoveMesh(IMesh* mesh) = 0;
	virtual const bool GetMeshes(std::vector<IMesh*>& meshes) = 0;
	
	virtual const bool AddTexture(ITexture* texture) = 0;
	virtual const bool RemoveTexture(ITexture* texture) = 0;
	virtual const bool GetTextures(std::vector<ITexture*>& textures) = 0;

	virtual const bool AddTextureToMesh(IMesh* mesh, ITexture* texture) = 0;
	virtual const bool RemoveTextureFromMesh(IMesh* mesh, ITexture* texture) = 0;
	virtual const bool GetMeshTextures(IMesh* mesh, std::vector<ITexture*>& textures) = 0;
};