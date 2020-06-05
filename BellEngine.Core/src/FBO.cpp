#include "FBO.h"
#include <BellEngineID.h>

const bool FBO::Shutdown()
{
	glDeleteTextures(1, &(this->colourTexture_0_ID));
	glDeleteTextures(1, &(this->depthTexture_ID));

	glDeleteFramebuffers(1, &(this->ID));

	return true;
}

void FBO::Dispose()
{
	DisposeVector(passIDs);
	//ClearBuffers();
	//Shutdown();
}

FBO::FBO()
{
	ID = (0);
	colourTexture_0_ID = (0);
	depthTexture_ID = (0); 
	width = (-1); 
	height = (-1);
};
FBO::~FBO()
{
	Dispose();
}

const bool FBO::Initialize(const int& width, const int& height)
{
	this->width = width;
	this->height = height;

	//	glCreateFramebuffers(1, &( this->ID ) );	// GL 4.5		//g_FBO
	glGenFramebuffers(1, &(this->ID));		// GL 3.0
	glBindFramebuffer(GL_FRAMEBUFFER, this->ID);

	//************************************************************
		// Create the colour buffer (texture)
	glGenTextures(1, &(this->colourTexture_0_ID));		//g_FBO_colourTexture
	glBindTexture(GL_TEXTURE_2D, this->colourTexture_0_ID);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8,		// 8 bits per colour
//	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F,		// 8 bits per colour
this->width,				// g_FBO_SizeInPixes
this->height);			// g_FBO_SizeInPixes

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//***************************************************************

		// Create the depth buffer (texture)
	glGenTextures(1, &(this->depthTexture_ID));			//g_FBO_depthTexture
	glBindTexture(GL_TEXTURE_2D, this->depthTexture_ID);

	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, ]

	// Note that, unless you specifically ask for it, the stencil buffer
	// is NOT present... i.e. GL_DEPTH_COMPONENT32F DOESN'T have stencil

	// These are:
	// - GL_DEPTH32F_STENCIL8, which is 32 bit float depth + 8 bit stencil
	// - GL_DEPTH24_STENCIL8,  which is 24 bit float depth + 8 bit stencil (more common?)
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8,	//GL_DEPTH32F_STENCIL8,
		this->width,		//g_FBO_SizeInPixes
		this->height);
	//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT );
	//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_COMPONENT );
	//	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, this->width, this->height, 0, GL_EXT_packe

	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH24_STENCIL8, GL_TEXTURE_2D, this->depthTexture_ID, 0);

	// ***************************************************************

	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,			// Colour goes to #0
		this->colourTexture_0_ID, 0);


	//	glFramebufferTexture(GL_FRAMEBUFFER,
	//						 GL_DEPTH_ATTACHMENT,
	//						 this->depthTexture_ID, 0);
	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_DEPTH_STENCIL_ATTACHMENT,
		this->depthTexture_ID, 0);

	static const GLenum draw_bufers[] =
	{
		GL_COLOR_ATTACHMENT0
	};
	glDrawBuffers(1, draw_bufers);		// There are 4 outputs now

	// ***************************************************************




	// ADD ONE MORE THING...
	bool bFrameBufferIsGoodToGo = true;

	switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
	case GL_FRAMEBUFFER_COMPLETE:
		bFrameBufferIsGoodToGo = true;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		SetLastErrorString("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
		bFrameBufferIsGoodToGo = false;
		break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
	case GL_FRAMEBUFFER_UNSUPPORTED:
	default:
		bFrameBufferIsGoodToGo = false;
		break;
	}//switch ( glCheckFramebufferStatus(GL_FRAMEBUFFER) )

	// Point back to default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return bFrameBufferIsGoodToGo;
}

// Calls shutdown(), then init()
const bool FBO::Reset(const int& width, const int& height)
{
	if (!this->Shutdown())
	{
		SetLastErrorString("Could not shutdown");
		return false;
	}

	return this->Initialize(width, height);
}

void FBO::ClearColourBuffer(const int& bufferindex)
{
	glViewport(0, 0, this->width, this->height);
	GLfloat	zero = 0.0f;
	glClearBufferfv(GL_COLOR, bufferindex, &zero);

	return;
}

void FBO::ClearBuffers(const bool& bClearColour, const bool& bClearDepth)
{
	glViewport(0, 0, this->width, this->height);
	GLfloat	zero = 0.0f;
	GLfloat one = 1.0f;
	if (bClearColour)
	{
		glClearBufferfv(GL_COLOR, 0, &zero);		// Colour
	}
	if (bClearDepth)
	{
		glClearBufferfv(GL_DEPTH, 0, &one);		// Depth is normalized 0.0 to 1.0f
	}
	// If buffer is GL_STENCIL, drawbuffer must be zero, and value points to a 
	//  single value to clear the stencil buffer to. Masking is performed in the 
	//  same fashion as for glClearStencil. Only the *iv forms of these commands 
	//  should be used to clear stencil buffers; be used to clear stencil buffers; 
	//  other forms do not accept a buffer of GL_STENCIL.

	// 
	glStencilMask(0xFF);

	{	// Clear stencil
		//GLint intZero = 0;
		//glClearBufferiv(GL_STENCIL, 0, &intZero );
		glClearBufferfi(GL_DEPTH_STENCIL,
			0,		// Must be zero
			1.0f,	// Clear value for depth
			0);	// Clear value for stencil
	}

	return;
}

const int FBO::GetMaxColourAttachments()
{
	//  void glGetIntegerv(GLenum pname,
	// 				       GLint * data);

	int maxColourAttach = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColourAttach);

	return maxColourAttach;
}

const int FBO::GetMaxDrawBuffers()
{
	int maxDrawBuffers = 0;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);

	return maxDrawBuffers;
}

const unsigned long long FBO::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_FBO;
}

const std::string FBO::GetType()
{
	return "FBO";
}

void FBO::SetTarget(const std::string& gameObject)
{
	target = gameObject;
}

const std::string FBO::GetTarget()
{
	return target;
}

void FBO::SetParent(const std::string& parent)
{
	this->parent = parent;
}

const std::string FBO::GetParent()
{
	return parent;
}

void FBO::SetCameraReference(const std::string& cameraName)
{
	this->cameraReference = cameraName;
}

const std::string FBO::GetCameraReference()
{
	return cameraReference;
}

const bool FBO::AddPass(const int& passID, const bool& enabled)
{
	auto it = std::find_if(passIDs.begin(), passIDs.end(), [passID](PassInfo_s* pass) { return pass->id == passID; });
	if (it == passIDs.end())
	{
		PassInfo_s* pass = new PassInfo_s();
		pass->id = passID;
		pass->enabled = enabled;

		passIDs.push_back(pass);
		return true;
	}

	return false;
}

const bool FBO::RemovePassID(const int& passID)
{
	auto it = std::find_if(passIDs.begin(), passIDs.end(), [passID](PassInfo_s* pass) { return pass->id == passID; });
	if (it != passIDs.end())
	{
		passIDs.erase(it);
		return true;
	}

	return false;
}

const bool FBO::SetPassState(const int& passID, const bool& enabled)
{
	auto it = std::find_if(passIDs.begin(), passIDs.end(), [passID](PassInfo_s* pass) { return pass->id == passID; });
	if (it != passIDs.end())
	{
		(*it)->enabled = enabled;
		return true;
	}

	return false;
}

const bool FBO::GetPassState(const int& passID, bool& enabled)
{
	auto it = std::find_if(passIDs.begin(), passIDs.end(), [passID](PassInfo_s* pass) { return pass->id == passID; });
	if (it != passIDs.end())
	{
		enabled = (*it)->enabled;
		return true;
	}

	return false;
}

const bool FBO::GetPasses(std::vector<std::pair<int, bool>>& passes, const bool& onlyEnabled)
{
	size_t vecsz = this->passIDs.size();

	for (size_t i = 0; i < vecsz; i++)
	{
		if (onlyEnabled)
		{
			if (!this->passIDs[i]->enabled)
			{
				continue;
			}
		}
		
		passes.push_back(std::pair<int, bool>(this->passIDs[i]->id, this->passIDs[i]->enabled));
	}

	return !passes.empty();
}

const std::string FBO::GetName()
{
	return this->name;
}

void FBO::SetName(const std::string& name)
{
	this->name = name;
}
