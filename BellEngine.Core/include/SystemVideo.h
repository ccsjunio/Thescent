/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <GLCommon.h>
#include <MathLibrary.h>
#include <map>
#include "Shader.h"
#include "ShaderProgram.h"
#include <cstdarg>
#include "../DebugRenderer/cDebugRenderer.h"
#include "IDisposable.h"
#include <ISystem.h>
#include <ModelDef.h>
#include "FBO.h"
#include "IExtendedInfo.h"
#include "Scene.h"
#include <ISystemVideo.h>
#include "TextureManager.h"
#include <Triangle3D.h>

#define DEFAULT_FRAMEBUFFER (GLuint)0

class SystemVideo : public ISystemVideo, public IDisposable
{
protected:
	Scene* scene;
	GLFWwindow* window;
	std::map<std::string, ICamera*> cameraReferenceMap;
	std::map<std::string, IGraphicsBuffer*> graphicBufferReferenceMap;
	std::map<std::string, ModelDrawInfo> VAOMap;
	bool isDebugRenderingActive;

	Vector4D debugRenderingColor;
	TextureManager* textureManager;
	cDebugRenderer* debugRenderer;

	std::map<unsigned int, ITexture*> loadedTextures;
	void LoadTextures(Entity* gameObject);

	const bool RegisterCamera(ICamera* camera);
	unsigned int GetLastTextureIndex();
	unsigned int GetLoadedTextureIndex(std::string texture);
	ITexture* FindLoadedTextureByPath(const std::string& texture);
	bool BindFrameBuffer(FBO* frameBuffer = nullptr);
	
	void DisposeShaderPrograms();
	SystemVideo();

public:
	int windowHeight;
	int windowWidth;
	float aspectRatio;

	virtual ~SystemVideo() {};

	TextureManager* GetTextureManager();
	const bool ClearFrameBuffers();
	void SetDebugRenderingColor(const Vector4D& color);
	void ToggleDebugRendering(const bool& option);
	void SetScene(Scene* scene);
	void SetWindow(GLFWwindow* window);
	void SetWindowTitle(const std::string& title);
	void CenterWindow();
	static void CenterWindow(GLFWwindow* window, GLFWmonitor* monitor);
	static const int mini(const int& x, const int& y);
	static const int maxi(const int& x, const int& y);
	GLFWmonitor* GetCurrentMonitor();
	static GLFWmonitor* GetCurrentMonitor(GLFWwindow* window);
	const bool RegisterFrameBuffer(IGraphicsBuffer* frameBuffer);
	bool RenderCollider(ILight* light, const float& lifetime = 0.0f);
	bool RenderCollider(Entity* entity, const float& lifetime = 0.0f);
	void AddDebugCapsule(const std::vector<Vector3D>& vertices, const Vector4D& color, const float& lifeTime = 0.0f);
	void AddDebugSphere(const Vector3D& center, const float& radius, const Vector4D& color, const float& lifeTime = 0.0f);
	void AddDebugPlane(const Vector3D& normal, const Vector3D& center, const std::vector<Vector3D>& vertices, const Vector4D& color, const float& lifeTime = 0.0f);
	void AddDebugLine(const Vector3D& startPoint, const Vector3D& endPoint, const Vector4D& color, const float& lifeTime = 0.0f);
	void AddDebugPoint(const Vector3D& point, const Vector4D& color, const float& lifeTime = 0.0f, const float& size = 1.0f);
	void AddDebugParallelepiped(const std::vector<Vector3D>& vertices, const Vector4D& color, const float& lifeTime = 0.0f);
	void AddDebugTriangle(Triangle3D& triangle, const Vector4D& color, const float& lifeTime = 0.0f);
	void SetWindowSize(const int& windowWidth, const int& windowHeight);
	void DrawDebugObjects();	
	const bool IsTransparent(Entity*& entity);
	const bool TransparencySort(std::vector<Entity*>& orderedEntities);
	
	const bool SetFBOPassState(const std::string& bufferName, const int& passID, const bool& passState);
	const bool GetFBOPassState(const std::string& bufferName, const int& passID, bool& passState);
};

