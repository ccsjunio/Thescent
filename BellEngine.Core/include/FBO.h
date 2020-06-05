#pragma once
#include <string>
#include <GLCommon.h>
#include <IExtendedInfo.h>
#include <IGraphicsBuffer.h>
#include <IDisposable.h>

class FBO : public IGraphicsBuffer, public IExtendedInfo, public IDisposable
{
private:
	struct PassInfo_s
	{
		int id;
		bool enabled;
	};

	std::string name;
	std::vector<PassInfo_s*> passIDs;
	std::string parent;
	std::string target;
	std::string cameraReference;
	const bool Shutdown();

public:
	//GLuint TBDTexture_1_ID;
	//GLuint TBDTexture_2_ID;
	GLuint ID;						// = 0;
	GLuint colourTexture_0_ID;		// = 0;
	GLuint depthTexture_ID;		// = 0;
	GLint width;		// = 512 the WIDTH of the framebuffer, in pixels;
	GLint height;

	FBO();
	~FBO();
		
	// Inits the FBP
	const bool Initialize(const int& width, const int& height);	
	// Calls shutdown(), then init()
	const bool Reset(const int& width, const int& height);
	void ClearBuffers(const bool& bClearColour = true, const bool& bClearDepth = true);
	void ClearColourBuffer(const int& bufferindex);
	const int GetMaxColourAttachments();
	const int GetMaxDrawBuffers();
	const unsigned long long GetTypeID();
	const std::string GetType();
	void SetTarget(const std::string& gameObject);
	const std::string GetTarget();
	void SetParent(const std::string& parent);
	const std::string GetParent();
	void SetCameraReference(const std::string& cameraName);
	const std::string GetCameraReference();
	const bool AddPass(const int& passID, const bool& enabled = true);
	const bool RemovePassID(const int& passID);
	const bool SetPassState(const int& passID, const bool& enabled);
	const bool GetPassState(const int& passID, bool& enabled);
	const bool GetPasses(std::vector<std::pair<int, bool>>& passes, const bool& onlyEnabled = true);
	const std::string GetName();
	void SetName(const std::string& name);

	void Dispose();
};