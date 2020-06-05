#pragma once
#include <SystemVideo.h>
#include <Level.h>
#include <ISingleton.h>
#include <StencilBuffer.h>
#include <IMesh.h>
#include <ShaderFeature.h>
#include "IGLShader.h"
#include <IMeshComponent.h>
#include <IMesh.h>
#include <functional>
#include <IFactory.h>
#include <IShaderContainer.h>
#include <atomic>

class GameSystemVideo : public SystemVideo, public ISingleton
{
private:
	enum class SystemFlags_e
	{
		None,

		DonotDrawEntitiesNotFacingCamera
	};

	static GameSystemVideo* systemVideo;
	std::atomic<bool> isActive, isUpdateComplete;
	IShaderContainer* shaderContainer;
	std::atomic<unsigned long long> systemFlags;

	std::function<void(const unsigned int&, const unsigned int&)> onWindowResizedEvent;

	std::vector<IGLShader*> glShaders;

	GameSystemVideo();

	const bool ResolveCamera(const unsigned int & programID);

	void CreateGLShaders();

	void WriteShaderFeatures(std::vector<ShaderFeature*>& features, IMesh*& mesh, IMeshComponent*& meshComponent);
	void WriteShaderFeatures(Entity*& entity, IMesh*& mesh, IMeshComponent*& meshComponent);
	void DrawParticles(Entity*& entity);\
	void DrawFBO(Entity*& entity, FBO*& framebuffer);
	void DrawStencil(Entity*& entity, StencilBuffer* framebuffer);
	void SetGLCallbacks();

public:	
	~GameSystemVideo();

	void SetSystemFlags(const unsigned long long& flags);
	void SetShaderContainer(IShaderContainer*& value);
	IShaderContainer* GetShaderContainer();
	const bool IsUpdateComplete();
	const bool IsActive();
	void Activate(const bool& value = true);
	void UpdateCamera(ICamera*& camera, const unsigned int& programID);
	void BindOnWindowResizeEvent(std::function<void(const unsigned int&, const unsigned int&)> fn);
	void DrawObject(Entity*& entity);
	void Initialize();
	static void* GetInstance();
	static void* GetInstance(unsigned int numberOfArguments, ...);
	const bool FindModelDrawInfoByMeshName(const std::string& name, ModelDrawInfo& modelDrawInfo);
	const bool CreateModelDrawInfo(IMesh*& mesh, ModelDrawInfo& modelDrawInfo);
	const bool LoadModelDrawInfoToVAO(ModelDrawInfo& modelDrawInfo);
	void SetupEntities(Entity*& entity);
	void SetupLevel(Level*& level);
	const bool DrawScene(FBO* destination = nullptr);
	void Update(const FrameTime& deltaTime);
	void Dispose();
};

