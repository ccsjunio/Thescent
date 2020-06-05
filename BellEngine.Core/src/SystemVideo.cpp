/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#include <GLCommon.h>
#include <SystemVideo.h>
#include <sstream>
#include "ShaderCompiler.h"
#include <EngineFolders.h>
#include "ModelDef.h"
#include <StringHelpers.h>
#include <IRigidbody.h>
#include "ShaderProgramFactory.h"
#include <IRender.h>
#include <IMeshComponent.h>
#include <Triangle3D.h>
#include <BellEngineID.h>

void SystemVideo::LoadTextures(Entity* entity)
{
	unsigned int startIndex = 0;
	unsigned int lastIndex = GetLastTextureIndex();
	if (lastIndex != UINT32_MAX)
	{
		startIndex = lastIndex + 1;
	}

	//Loads a rigidbody texture
	if (entity->GetTypeID() == ENGINE_OBJ_ID_SCN_GAMEOBJECT)
	{
		std::vector<IMeshComponent*> mCs;
		if (entity->GetComponents(mCs))
		{
			for (IMeshComponent* mC : mCs)
			{
				std::vector<IMesh*> meshes;
				if (mC->GetMeshes(meshes))
				{
					for (size_t meshIndex = 0; meshIndex < meshes.size(); meshIndex++)
					{
						IMesh* mesh = meshes[meshIndex];

						std::vector<ITexture*> textures;
						if (mC->GetMeshTextures(mesh, textures))
						{
							for (unsigned int i = 0; i < textures.size(); i++)
							{
								if (!FindLoadedTextureByPath(textures[i]->path))
								{
									if (textureManager->Create2DTexture(textures[i], true))
									{
										loadedTextures[startIndex + i] = textures[i];
									}
									else
									{
										printf("Could not load 2D texture '%s'.\n", textures[i]->path.c_str());
									}
								}
								else
								{
									//Texture was already loaded.
								}								
							}
						}
					}
				}
			}
		}
	}
	//Loads a skybox texture
	else if (entity->GetTypeID() == ENGINE_OBJ_ID_SCN_SKYBOX)
	{
		IMeshComponent* mC = entity->GetComponent<IMeshComponent*>();
		if (mC)
		{
			std::vector<IMesh*> meshes;
			if (mC->GetMeshes(meshes))
			{
				IMesh* mesh = meshes[0];

				std::vector<ITexture*> textures;
				if (mC->GetMeshTextures(mesh, textures))
				{
					if (!FindLoadedTextureByPath(textures[0]->path))
					{
						unsigned int id;
						std::string errorString;
						if (textureManager->CreateCubeMapTexture(textures[0], true/*, errorString*/))
						{
							loadedTextures[startIndex] = textures[0];
						}
						else
						{
							printf("Could not load skybox texture '%s'.\n", entity->GetName().c_str());
						}
					}
					else
					{
						//Texture already loaded
					}					
				}
				else
				{
					printf("The skybox mesh has no textures!\n");
				}
			}
			else
			{
				printf("Could not find skybox mesh for '%s'.\n", entity->GetName().c_str());
			}
		}
		else
		{
			printf("Could not find skybox mesh for '%s'.\n", entity->GetName().c_str());
		}
	}
	else
	{
		//Do nothing
	}
}

const bool SystemVideo::RegisterCamera(ICamera* camera)
{
	if (camera)
	{
		auto it = cameraReferenceMap.find(camera->GetName());
		if (it == cameraReferenceMap.end())
		{
			cameraReferenceMap[camera->GetName()] = camera;
			return true;
		}
	}

	return false;
}

unsigned int SystemVideo::GetLastTextureIndex()
{
	unsigned int index = 0;

	if (loadedTextures.size() != 0)
	{
		std::map<unsigned int, ITexture*>::iterator it = loadedTextures.end();

		it--;

		index = (*it).first;
	}
	else
	{
		index = UINT32_MAX;
	}

	return index;;
}

unsigned int SystemVideo::GetLoadedTextureIndex(std::string texture)
{
	bool rc = false;
	unsigned int index = 0;

	std::map<unsigned int, ITexture*>::iterator it = loadedTextures.begin();
	for (std::pair<unsigned int, ITexture*> element : loadedTextures)
	{
		if (element.second->path == texture)
		{
			index = element.first;
			rc = true;
			break;
		}
	}

	return rc ? index : UINT32_MAX;
}

ITexture* SystemVideo::FindLoadedTextureByPath(const std::string& texturepath)
{
	ITexture* ptr = nullptr;

	for (auto element : loadedTextures)
	{
		if (element.second->path == texturepath)
		{
			ptr = element.second;
			break;
		}
	}

	return ptr;
}

bool SystemVideo::BindFrameBuffer(FBO* frameBuffer)
{
	bool rc = false;

	//Checks if we're trying to use a framebuffer other than the default one.
	if (frameBuffer)
	{
		//Bind custom FBO to draw call
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->ID);
		frameBuffer->ClearBuffers(true, true);
		rc = true;
	}
	else
	{
		//Bind default screen framebuffer (0) to draw call
		glBindFramebuffer(GL_FRAMEBUFFER, DEFAULT_FRAMEBUFFER);
		rc = true;
	}

	return rc;
}

bool SystemVideo::RenderCollider(ILight* light, const float& lifetime)
{
	if (isDebugRenderingActive)
	{
		if (light)
		{
			ICollider* collider = light->GetCollider();
			if (collider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE)
			{
				float radius = 0.0f;
				Vector3D center = Vector3D();
				ParameterContainer params;
				Parameter p;

				bool rcC = collider->GetContent("radius", params);
				if (params.FindParameterByName("radius", p))
				{
					radius = *(float*)p.valuePtr;
				}

				params.Clear();

				rcC = collider->GetContent("center", params);
				if (params.FindParameterByName("center", p))
				{
					center = *(Vector3D*)p.valuePtr;
				}

				AddDebugSphere(center, radius, debugRenderingColor, lifetime);
			}
			else
			{
				SetLastErrorString("Light collider " + collider->GetType() + "is not supported.");
			}
		}
		else
		{
			//Jump
		}
	}

	return isDebugRenderingActive;
}

bool SystemVideo::RenderCollider(Entity* entity, const float& lifetime)
{
	IRigidbody* rb = entity->GetComponent<IRigidbody*>();
	if (rb)
	{
		ICollider* collider = rb->GetCollider();
		if (collider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE)
		{
			float radius = 0.0f;
			Vector3D center = Vector3D();
			ParameterContainer params;
			Parameter p;

			bool rcC = collider->GetContent("radius", params);
			if (params.FindParameterByName("radius", p))
			{
				radius = *(float*)p.valuePtr;
			}

			center = collider->GetTransformedProperties()->GetTransform()->GetPosition();

			AddDebugSphere(center, radius, debugRenderingColor, lifetime);
		}
		else if (collider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERCAPSULE)
		{
			std::vector<Vector3D> extremes;
			if (collider->GetTransformedProperties()->GetExtremes(extremes))
			{
				AddDebugCapsule(extremes, debugRenderingColor, lifetime);
			}
		}
		else if (collider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERBOX)
		{
			ParameterContainer params;

			if (collider->GetContent("vertices", params))
			{
				Parameter p;
				std::vector<Vector3D> vertices;
				Vector3D* verticesPtr;
				unsigned int verticesCount = 0;
				bool rc = false;

				rc = params.FindParameterByName("vertices", p);
				if (!rc) return false;
				verticesPtr = (Vector3D*)p.valuePtr;

				rc = params.FindParameterByName("verticesCount", p);
				if (!rc) return false;
				verticesCount = (unsigned int)p.valuePtr;

				if (verticesPtr)
				{
					for (unsigned int i = 0; i < verticesCount; i++) vertices.push_back(verticesPtr[i]);

					AddDebugParallelepiped(vertices, debugRenderingColor, lifetime);
				}
			}
		}
		else if (collider->GetTypeID() == ENGINE_OBJ_ID_PHY_COLLIDERPLANE)
		{
			Vector3D center, normal = Vector3D();
			std::vector<Vector3D> vertices;
			ParameterContainer params;
			Parameter p;

			bool rcC = collider->GetContent("vertices", params);
			if (params.FindParameterByName("vertices", p))
			{
				vertices = *(std::vector<Vector3D>*)p.valuePtr;
			}

			params.Clear();

			rcC = collider->GetContent("normal", params);
			if (params.FindParameterByName("normal", p))
			{
				normal = *(Vector3D*)p.valuePtr;
			}

			center = collider->GetTransformedProperties()->GetTransform()->GetPosition();

			AddDebugPlane(normal, center, vertices, debugRenderingColor, lifetime);
		}
		else
		{
			SetLastErrorString("Could not render collider type " + collider->GetType());
		}

		return true;
	}

	return false;
}

void SystemVideo::DisposeShaderPrograms()
{
	
}

SystemVideo::SystemVideo()
{
	scene = nullptr;
	window = nullptr;
	debugRenderingColor = Vector4D(0.0f, 1.0f, 0.0f, 1.0f);
	isDebugRenderingActive = false;

	windowHeight = 0;
	windowWidth = 0;
	aspectRatio = 0.0f;

	textureManager = new TextureManager();
	debugRenderer = new cDebugRenderer();
	debugRenderer->initialize();
}

TextureManager* SystemVideo::GetTextureManager()
{
	return this->textureManager;
}

const bool SystemVideo::ClearFrameBuffers()
{
	for (auto element : graphicBufferReferenceMap)
	{
		if (element.second->GetType() == "FBO")
		{
			if (element.second)
			{
				FBO* fbo = (FBO*)element.second;
				//CLEAR BUFFERS
				glViewport(0, 0, fbo->width, fbo->height);
				GLfloat	zero = 0.0f;
				GLfloat one = 1.0f;
				glClearBufferfv(GL_COLOR, 0, &zero);		// Colour
				glClearBufferfv(GL_DEPTH, 0, &one);		// Depth is normalized 0.0 to 1.0f
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

				//SHUTDOWN
				glDeleteTextures(1, &(fbo->colourTexture_0_ID));
				glDeleteTextures(1, &(fbo->depthTexture_ID));
				glDeleteFramebuffers(1, &(fbo->ID));
			}
		}
	}

	graphicBufferReferenceMap.clear();

	return true;
}

void SystemVideo::SetDebugRenderingColor(const Vector4D& color)
{
	debugRenderingColor = color;
}

void SystemVideo::ToggleDebugRendering(const bool& option)
{
	isDebugRenderingActive = option;
}

void SystemVideo::SetScene(Scene* scene)
{
	this->scene = scene;

	/*Registers the camera of the scene*/
	RegisterCamera(scene->GetCamera());
}

void SystemVideo::SetWindow(GLFWwindow* window)
{
	this->window = window;
}

void SystemVideo::SetWindowTitle(const std::string& title)
{
	glfwSetWindowTitle(this->window, title.c_str());
}

void SystemVideo::CenterWindow()
{
	SystemVideo::CenterWindow(this->window, glfwGetPrimaryMonitor());
}

void SystemVideo::CenterWindow(GLFWwindow* window, GLFWmonitor* monitor)
{
	if (!monitor)
	{
		return;
	}

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	if (!mode)
	{
		return;
	}

	int monitorX, monitorY;
	glfwGetMonitorPos(monitor, &monitorX, &monitorY);

	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	glfwSetWindowPos(window, monitorX + (mode->width - windowWidth) / 2, monitorY + (mode->height - windowHeight) / 2);
}

const bool SystemVideo::RegisterFrameBuffer(IGraphicsBuffer* frameBuffer)
{
	bool rc = false;

	if (frameBuffer)
	{
		if (frameBuffer->GetType() == "FBO")
		{
			rc = ((FBO*)frameBuffer)->Initialize(windowWidth, windowHeight);
			if (rc)
			{
				graphicBufferReferenceMap[frameBuffer->GetName()] = frameBuffer;
			}
			else
			{
				SetLastErrorString(((FBO*)frameBuffer)->GetLastErrorString());
			}
		}
		else if (frameBuffer->GetType() == "StencilBuffer")
		{
			graphicBufferReferenceMap[frameBuffer->GetName()] = frameBuffer;
		}
		else
		{

		}
	}
	else
	{
		SetLastErrorString("RegisterFrameBuffer: FrameBuffer is null!");
	}

	return rc;
}

void SystemVideo::AddDebugCapsule(const std::vector<Vector3D>& vertices, const Vector4D& color, const float& lifeTime)
{
	/*Top dome*/
	AddDebugLine(vertices[0], vertices[1], color, lifeTime);
	AddDebugLine(vertices[0], vertices[2], color, lifeTime);
	AddDebugLine(vertices[0], vertices[3], color, lifeTime);
	AddDebugLine(vertices[0], vertices[4], color, lifeTime);

	/*Bottom dome*/
	AddDebugLine(vertices[5], vertices[6], color, lifeTime);
	AddDebugLine(vertices[5], vertices[7], color, lifeTime);
	AddDebugLine(vertices[5], vertices[8], color, lifeTime);
	AddDebugLine(vertices[5], vertices[9], color, lifeTime);

	/*Body*/
	AddDebugLine(vertices[1], vertices[6], color, lifeTime);
	AddDebugLine(vertices[2], vertices[7], color, lifeTime);
	AddDebugLine(vertices[3], vertices[8], color, lifeTime);
	AddDebugLine(vertices[4], vertices[9], color, lifeTime);
}

void SystemVideo::AddDebugSphere(const Vector3D& center, const float& radius, const Vector4D& color, const float& lifeTime)
{
	//Line in X
	Vector3D p0 = center;
	p0.x = center.x - radius;
	Vector3D p1 = center;
	p1.x = center.x + radius;
	AddDebugLine(p0, p1, color, lifeTime);
	//Line in Y
	Vector3D p2 = center;
	p2.y = center.y - radius;
	Vector3D p3 = center;
	p3.y = center.y + radius;
	AddDebugLine(p2, p3, color, lifeTime);
	//Line in Z
	Vector3D p4 = center;
	p4.z = center.z - radius;
	Vector3D p5 = center;
	p5.z = center.z + radius;
	AddDebugLine(p4, p5, color, lifeTime);
}

void SystemVideo::AddDebugPlane(const Vector3D& normal, const Vector3D& center, const std::vector<Vector3D>& vertices, const Vector4D& color, const float& lifeTime)
{
	if (vertices.size() < 4)
	{
		throw std::exception("Must have at least 4 vertices to plot 4 lines.");
	}

	//Normal
	AddDebugLine(center, center + normal, color, lifeTime);

	//4 lines
	//AB
	AddDebugLine(vertices[0], vertices[1], color, lifeTime);
	//BC
	AddDebugLine(vertices[1], vertices[2], color, lifeTime);
	//CD
	AddDebugLine(vertices[2], vertices[3], color, lifeTime);
	//DA
	AddDebugLine(vertices[3], vertices[0], color, lifeTime);
}

void SystemVideo::AddDebugLine(const Vector3D& startPoint, const Vector3D& endPoint, const Vector4D& color, const float& lifeTime)
{
	debugRenderer->addLine(startPoint, endPoint, Vector3D(color.r, color.g, color.b), lifeTime);
}

void SystemVideo::AddDebugPoint(const Vector3D& point, const Vector4D& color, const float& lifeTime, const float& size)
{
	debugRenderer->addPointPointSize(point, Vector3D(color.r, color.g, color.b), lifeTime, size);
}

void SystemVideo::AddDebugParallelepiped(const std::vector<Vector3D>& vertices, const Vector4D& color, const float& lifeTime)
{
	if (vertices.size() < 8)
	{
		throw std::exception("Must have at least 8 vertices to plot 12 lines.");
	}

	//12 lines
	//p0 - p1
	AddDebugLine(vertices[0], vertices[1], color, lifeTime);
	//p0 - p2
	AddDebugLine(vertices[0], vertices[2], color, lifeTime);
	//p0 - p6
	AddDebugLine(vertices[0], vertices[6], color, lifeTime);
	//p1 - p3
	AddDebugLine(vertices[1], vertices[3], color, lifeTime);
	//p1 - p7
	AddDebugLine(vertices[1], vertices[7], color, lifeTime);
	//p2 - p3
	AddDebugLine(vertices[2], vertices[3], color, lifeTime);
	//p2 - p5
	AddDebugLine(vertices[2], vertices[5], color, lifeTime);
	//p3 - p4
	AddDebugLine(vertices[3], vertices[4], color, lifeTime);
	//p4 - p5
	AddDebugLine(vertices[4], vertices[5], color, lifeTime);
	//p4 - p7
	AddDebugLine(vertices[4], vertices[7], color, lifeTime);
	//p5 - p6
	AddDebugLine(vertices[5], vertices[6], color, lifeTime);
	//p6 - p7
	AddDebugLine(vertices[6], vertices[7], color, lifeTime);
}

void SystemVideo::AddDebugTriangle(Triangle3D& triangle, const Vector4D& color, const float& lifeTime)
{
	AddDebugLine(triangle.pointA.GetVec3(), triangle.pointB.GetVec3(), color, lifeTime);
	AddDebugLine(triangle.pointB.GetVec3(), triangle.pointC.GetVec3(), color, lifeTime);
	AddDebugLine(triangle.pointC.GetVec3(), triangle.pointA.GetVec3(), color, lifeTime);

	/*debugRenderer->addTriangle
	(
		triangle.pointA.GetVec3(),
		triangle.pointB.GetVec3(),
		triangle.pointC.GetVec3(),
		Vector3D(color.r, color.g, color.b),
		lifeTime
	);*/
}

void SystemVideo::SetWindowSize(const int& windowWidth, const int& windowHeight)
{
	this->windowHeight = windowHeight;
	this->windowWidth = windowWidth;
	this->aspectRatio = (float)windowWidth / (float)windowHeight;

	glfwSetWindowSize(window, windowWidth, windowHeight);
	CenterWindow();
}

void SystemVideo::DrawDebugObjects()
{
	/*if (!isDebugRenderingActive)
	{
		return;
	}*/

	if (!scene)
	{
		SetLastErrorString("Could not find scene!");
		return;
	}

	ICamera* camera = scene->GetCamera();

	if (!camera)
	{
		SetLastErrorString("Could not find camera!");
		return;
	}

	/*Protection for the AR*/
	if (std::isnan(aspectRatio)) return;

	debugRenderer->RenderDebugObjects(camera->GetViewMatrix(), camera->GetProjectionMatrix(aspectRatio), frameTime.GetDeltaTime());
}

const bool SystemVideo::IsTransparent(Entity*& entity)
{
	IRender* vr = entity->GetComponent<IRender*>();
	if (vr)
	{
		return vr->GetDiffuse().a < 1.0f;
	}

	return false;
}

const bool SystemVideo::TransparencySort(std::vector<Entity*>& orderedEntities)
{
	Scene* scene = this->scene;

	std::vector<Entity*>* entitiesPtr = scene->GetEntities();
	if (entitiesPtr)
	{
		std::vector<Entity*> entities = *entitiesPtr;
		/*Performance improvement.*/
		/*This method is called every frame, multiple times.
		We reserve the output vector with the maximum number of entities possible,
		so the "push_back" function does not have to reallocate memory for each insertion.*/
		size_t maxEntities = entities.size();
		orderedEntities.reserve(maxEntities);

		//Gets all non-transparent objects
		for (auto& entity : entities)
		{
			/*Don't care about disposing entites.*/
			//if (!entity || entity->IsDisposing()) continue;
			/*If not transparent, then add to the list.*/
			if (!IsTransparent(entity))
			{
				orderedEntities.push_back(entity);
			}
		}

		//Orders entities by distance to the camera 
		std::sort
		(
			orderedEntities.begin(),
			orderedEntities.end(),
			[&scene](const auto& a, const auto& b) -> bool
			{
				//if (!a || !b) return false;
				//if (a->IsDisposing() || b->IsDisposing()) return false;

				IRigidbody* aRB = a->GetComponent<IRigidbody*>();
				IRigidbody* bRB = b->GetComponent<IRigidbody*>();
				if (!aRB || !bRB) return false;

				return
					glm::distance(aRB->GetPosition(), scene->GetCamera()->GetEyePosition()) <
					glm::distance(bRB->GetPosition(), scene->GetCamera()->GetEyePosition());
			}
		);

		//Adds transparent objects at the end of the list
		for (auto& entity : entities)
		{
			/*Don't care about disposing entites.*/
			//if (!entity || entity->IsDisposing()) continue;

			if (IsTransparent(entity))
			{
				orderedEntities.push_back(entity);
			}
		}

		return true;
	}

	return false;
}

const int SystemVideo::mini(const int& x, const int& y)
{
	return x < y ? x : y;
}

const int SystemVideo::maxi(const int& x, const int& y)
{
	return x > y ? x : y;
}

GLFWmonitor* SystemVideo::GetCurrentMonitor()
{
	return SystemVideo::GetCurrentMonitor(this->window);
}

GLFWmonitor* SystemVideo::GetCurrentMonitor(GLFWwindow* window)
{
	int nmonitors, i;
	int wx, wy, ww, wh;
	int mx, my, mw, mh;
	int overlap, bestoverlap;
	GLFWmonitor* bestmonitor;
	GLFWmonitor** monitors;
	const GLFWvidmode* mode;

	bestoverlap = 0;
	bestmonitor = NULL;

	glfwGetWindowPos(window, &wx, &wy);
	glfwGetWindowSize(window, &ww, &wh);
	monitors = glfwGetMonitors(&nmonitors);

	for (i = 0; i < nmonitors; i++)
	{
		mode = glfwGetVideoMode(monitors[i]);
		glfwGetMonitorPos(monitors[i], &mx, &my);
		mw = mode->width;
		mh = mode->height;

		overlap =
			maxi(0, mini(wx + ww, mx + mw) - maxi(wx, mx)) *
			maxi(0, mini(wy + wh, my + mh) - maxi(wy, my));

		if (bestoverlap < overlap) {
			bestoverlap = overlap;
			bestmonitor = monitors[i];
		}
	}

	return bestmonitor;
}

const bool SystemVideo::SetFBOPassState(const std::string& bufferName, const int& passID, const bool& passState)
{
	auto it = graphicBufferReferenceMap.find(bufferName);
	if (it != graphicBufferReferenceMap.end())
	{
		FBO* fbo = (FBO*)((*it).second);
		
		return fbo->SetPassState(passID, passState);
	}

	return false;
}

const bool SystemVideo::GetFBOPassState(const std::string& bufferName, const int& passID, bool& passState)
{
	auto it = graphicBufferReferenceMap.find(bufferName);
	if (it != graphicBufferReferenceMap.end())
	{
		FBO* fbo = (FBO*)((*it).second);

		return fbo->GetPassState(passID, passState);
	}

	return false;
}