/***********************************************************************
* Project           : Game
* Author            : Felipe Bellini
* Date created      : Oct. 12 - 2019
* Licensed under:	: MIT

************************************************************************/
#include "LevelFactory.h"
#include <EngineFolders.h>
#include <IRigidBody.h>
#include <CameraLockedAt.h>
#include <CameraFree.h>
#include <Animation.h>
#include <LightDirectional.h>
#include <LightSpot.h>
#include <LightPoint.h>
#include <ShaderFeature.h>
#include <Shader.h>
#include <SystemVideo.h>
#include <FBO.h>
#include <StringHelpers.h>
#if __cplusplus >= 201703L
	#include <filesystem>
#endif
#include <ParameterContainer.h>
#include <GraphicsBufferFactory.h>
#include <Animation.h>
#include <ISkinnedMesh.h>
#include <IRender.h>
#include <Cloth.h>
#include <BellEngineID.h>
#include <IParticleEmitter.h>

LevelFactory::LevelFactory()
{
	this->systemScriptingLua = nullptr;
	this->systemSound = nullptr;
	this->systemMesh = nullptr;
	this->systemPhysics = nullptr;
	this->systemAI = nullptr;
	this->componentFactory = new ComponentFactory();
	this->videoFactory = new VideoFactory();
}
LevelFactory::~LevelFactory()
{
	Dispose();
}

void LevelFactory::BindOnFileLoadCallback(std::function<void(const ISystemDataPersistency::OnFileLoad_s&)> fn)
{
	if (!fn) return;

	this->onFileLoadCallback = fn;
}

Level* LevelFactory::Create(ParameterContainer& parameters)
{
	bool rc = false;
	Parameter p;
	Level* level = nullptr;

	rc = parameters.FindParameterByName("filepath", p);
	if (!rc) return nullptr;

	if (p.value.empty()) return nullptr;

	level = GetLevel(p.value);

	if (onFileLoadCallback)
	{
		ISystemDataPersistency::OnFileLoad_s info;
		info.fileName = StringHelpers::GetFileNameFromFilePath(p.value);
		info.filePath = p.value;
		info.eventDescription = "level";
		info.fileObject = level;
		onFileLoadCallback(info);
	}

	return level;
}
const bool LevelFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	bool rc = false;
	Parameter p;

	rc = input.FindParameterByName("systemAI", p);
	if (!rc) return false;
	systemAI = (ISystemAI*)p.valuePtr;
	if (!systemAI) return false;

	rc = input.FindParameterByName("systemSound", p);
	if (!rc) return false;
	systemSound = (ISystemSound*)p.valuePtr;
	if (!systemSound) return false;

	rc = input.FindParameterByName("systemMesh", p);
	if (!rc) return false;
	systemMesh = (ISystemMesh*)p.valuePtr;
	if (!systemMesh) return false;

	rc = input.FindParameterByName("systemPhysics", p);
	if (!rc) return false;
	systemPhysics = (ISystemPhysics*)p.valuePtr;
	if (!systemPhysics) return false;

	rc = input.FindParameterByName("systemScriptingLua", p);
	if (!rc) return false;
	systemScriptingLua = (ISystemScripting*)p.valuePtr;
	if (!systemScriptingLua) return false;

	return true;
}

void LevelFactory::Dispose()
{
	DisposePointer(componentFactory);
	DisposePointer(videoFactory);
}

Level* LevelFactory::GetLevel(const std::string& levelPath)
{
	bool rc = false;

#if __cplusplus == 201703L
	rc = std::filesystem::exists(levelPath);
	if (!rc)
	{
		SetLastErrorString("The path to this level does not exist.");
		return nullptr;
	}
#endif

	Level* level = new Level();

	//load file
	rapidxml::file<> xmlFile(levelPath.c_str());
	rapidxml::xml_document<>* xml = new rapidxml::xml_document<>();
	if (xml)
	{
		char* xmlData = xmlFile.data();
		if (xmlData)
		{
			xml->parse<0>(xmlData);

			//find root
			rapidxml::xml_node<>* root = xml->first_node();
			if (root)
			{
				auto nameXML = root->first_attribute("Name");
				if (nameXML)
				{
					level->SetName(nameXML->value());
				}
				else
				{
					level->SetName("Default Level");
				}

				GetScenes(root, level);
				GetFrameBuffers(root, level);
			}
			else
			{
				//Root not found
			}
		}
		else
		{
			//No xml data
		}

		delete(xml);
	}
	else
	{
		//XML object not created
	}

	return level;
}
void LevelFactory::GetFramebufferReferences(rapidxml::xml_node<char>* root, Scene* scene)
{
	rapidxml::xml_node<char>* xmlFBOs = root->first_node("FrameBuffers");
	if (xmlFBOs)
	{
		//DrawToZero
		{
			rapidxml::xml_attribute<char>* xmlDTZ = xmlFBOs->first_attribute("DrawToZero");
			if (xmlDTZ)
			{
				scene->SetDrawToMainBuffer(atoi(xmlDTZ->value()) != 0);
			}
		}

		for (rapidxml::xml_node<>* xmlFBO = xmlFBOs->first_node("FrameBufferName"); xmlFBO; xmlFBO = xmlFBO->next_sibling())
		{
			std::string str = xmlFBO->value();
			scene->AppendFBOReference(str);
		}
	}
}
void LevelFactory::GetCamera(rapidxml::xml_node<char>* root, Scene* scene)
{
	rapidxml::xml_node<char>* xmlCamera = root->first_node("Camera");
	if (xmlCamera)
	{
		ICamera* camera = nullptr;

		//Type
		std::string camType;
		{
			rapidxml::xml_attribute<>* typeXML = xmlCamera->first_attribute("Type");
			if (typeXML)
			{
				camType = typeXML->value();

				if (camType == "CameraLockedAt")
				{
					camera = new CameraLockedAt();
				}
				else if (camType == "CameraFree")
				{
					camera = new CameraFree();
				}
				else
				{
					//Unknown Camera
					return;
				}
			}
			else
			{
				//Unknown Camera
				return;
			}
		}

		//Name
		{
			rapidxml::xml_attribute<>* xml_Name = xmlCamera->first_attribute("Name");
			if (xml_Name)
			{
				camera->SetName(xml_Name->value());
			}
		}

		//EyePosition
		{
			rapidxml::xml_node<char>* xml_eyePosition = xmlCamera->first_node("EyePosition");
			if (xml_eyePosition)
			{
				Vector3D eyePosition = Vector3D();

				rapidxml::xml_attribute<char>* xml_x = xml_eyePosition->first_attribute("X");
				if (xml_x)
				{
					eyePosition.x = (float)atof(xml_x->value());
				}
				rapidxml::xml_attribute<char>* xml_y = xml_eyePosition->first_attribute("Y");
				if (xml_y)
				{
					eyePosition.y = (float)atof(xml_y->value());
				}
				rapidxml::xml_attribute<char>* xml_z = xml_eyePosition->first_attribute("Z");
				if (xml_z)
				{
					eyePosition.z = (float)atof(xml_z->value());
				}

				camera->SetEyePosition(eyePosition);
			}
		}

		//FOV
		{
			rapidxml::xml_node<char>* xml_FOV = xmlCamera->first_node("FOV");
			if (xml_FOV)
			{
				camera->SetFOV((float)atof(xml_FOV->value()));
			}
		}

		//FarClippingPlane
		{
			rapidxml::xml_node<char>* xml_FarClippingPlane = xmlCamera->first_node("FarClippingPlane");
			if (xml_FarClippingPlane)
			{
				camera->SetFarClippingPlane((float)atof(xml_FarClippingPlane->value()));
			}
		}

		//NearClippingPlane
		{
			rapidxml::xml_node<char>* xml_NearClippingPlane = xmlCamera->first_node("NearClippingPlane");
			if (xml_NearClippingPlane)
			{
				camera->SetNearClippingPlane((float)atof(xml_NearClippingPlane->value()));
			}
		}

		//Properties according to types
		if (camType == "CameraLockedAt")
		{
			//Target
			{
				rapidxml::xml_node<char>* xmlItem = xmlCamera->first_node("Target");
				if (xmlItem)
				{
					Entity* go = scene->FindEntityByName(xmlItem->value());
					((CameraLockedAt*)camera)->SetTarget(go);
				}
			}
		}
		else if (camType == "CameraFree")
		{
			//Target
			{
				rapidxml::xml_node<char>* xml_targetPosition = xmlCamera->first_node("Target");
				if (xml_targetPosition)
				{
					Vector3D targetPosition = Vector3D();

					rapidxml::xml_attribute<char>* xml_x = xml_targetPosition->first_attribute("X");
					if (xml_x)
					{
						targetPosition.x = (float)atof(xml_x->value());
					}
					rapidxml::xml_attribute<char>* xml_y = xml_targetPosition->first_attribute("Y");
					if (xml_y)
					{
						targetPosition.y = (float)atof(xml_y->value());
					}
					rapidxml::xml_attribute<char>* xml_z = xml_targetPosition->first_attribute("Z");
					if (xml_z)
					{
						targetPosition.z = (float)atof(xml_z->value());
					}

					((CameraFree*)camera)->SetTarget(targetPosition);
				}
			}

			//Up
			{
				rapidxml::xml_node<char>* xml_Up = xmlCamera->first_node("Up");
				if (xml_Up)
				{
					Vector3D up = Vector3D();
					rapidxml::xml_attribute<char>* xml_x = xml_Up->first_attribute("X");
					if (xml_x)
					{
						up.x = (float)atof(xml_x->value());
					}
					rapidxml::xml_attribute<char>* xml_y = xml_Up->first_attribute("Y");
					if (xml_y)
					{
						up.y = (float)atof(xml_y->value());
					}
					rapidxml::xml_attribute<char>* xml_z = xml_Up->first_attribute("Z");
					if (xml_z)
					{
						up.z = (float)atof(xml_z->value());
					}
					camera->SetUp(up);
				}
			}
		}

		scene->SetCamera(camera);

		//Extra step for transparency
		if (camera->GetTypeID() == ENGINE_OBJ_ID_GPH_CAMERALOCKEDAT)
		{
			auto target = ((CameraLockedAt*)camera)->GetTarget();
			if (target)
			{
				IRender* vr = target->GetComponent<IRender*>();
				if (vr)
				{
					if (vr->GetDiffuse().a < 1.0f)
					{
						scene->Remove(target, false);
						scene->Append(target);						
					}
				}
			}
		}
	}
}
void LevelFactory::GetSounds(rapidxml::xml_node<char>* root, Scene* scene)
{
	auto xmlSounds = root->first_node("Sounds");
	if (xmlSounds)
	{
		for (rapidxml::xml_node<>* xmlSound = xmlSounds->first_node("Sound");
			xmlSound;
			xmlSound = xmlSound->next_sibling())
		{
			std::string name, path;
			unsigned int id = 0;
			int type = 0;
			Vector3D position = Vector3D(0.0f, 0.0f, 0.0f);
			Vector3D speed = Vector3D(0.0f, 0.0f, 0.0f);

			//ID
			{
				auto xml_ID = xmlSound->first_attribute("ID");
				if (xml_ID)
				{
					id = std::strtoul(xml_ID->value(), NULL, NULL);
				}
			}

			//Name
			{
				auto xml_item = xmlSound->first_attribute("Name");
				if (xml_item)
				{
					name = xml_item->value();
				}
			}

			//Path
			{
				auto xml_item = xmlSound->first_attribute("Path");
				if (xml_item)
				{
					path = EngineFolders::GetSoundsPath() + xml_item->value();
				}
			}

			//Flags
			{
				//auto xml_item = xmlSound->first_node("Flags");
				//if (xml_item)
				//{
				//	//unsigned int debug_light_ID = 0;
				//	for (rapidxml::xml_node<>* xml_flag = xml_item->first_node("Flag");
				//		xml_flag;
				//		xml_flag = xml_flag->next_sibling())
				//	{
				//		std::string flagStr = xml_flag->value();
				//		if (!flagStr.empty())
				//		{
				//			unsigned int flag = atol(flagStr.c_str());
				//			sound.flags.push_back(flag);
				//		}
				//	}
				//}
			}

			//Position
			{
				auto xml_Item = xmlSound->first_node("Position");
				if (xml_Item)
				{
					auto xml_x = xml_Item->first_attribute("X");
					if (xml_x)
					{
						position.x = (float)atof(xml_x->value());
					}
					auto xml_y = xml_Item->first_attribute("Y");
					if (xml_y)
					{
						position.y = (float)atof(xml_y->value());
					}
					auto xml_z = xml_Item->first_attribute("Z");
					if (xml_z)
					{
						position.z = (float)atof(xml_z->value());
					}
				}
			}

			//Speed
			{
				auto xml_Item = xmlSound->first_node("Speed");
				if (xml_Item)
				{
					auto xml_x = xml_Item->first_attribute("X");
					if (xml_x)
					{
						speed.x = (float)atof(xml_x->value());
					}
					auto xml_y = xml_Item->first_attribute("Y");
					if (xml_y)
					{
						speed.y = (float)atof(xml_y->value());
					}
					auto xml_z = xml_Item->first_attribute("Z");
					if (xml_z)
					{
						speed.z = (float)atof(xml_z->value());
					}
				}
			}

			//Type
			{
				auto xml_item = xmlSound->first_attribute("Type");
				if (xml_item)
				{
					type = std::strtol(xml_item->value(), NULL, 16);
				}
			}

			//ChannelGroupName
			{
				/*	auto xml_item = xmlSound->first_node("ChannelGroupName");
					if (xml_item)
					{
						sound.groupName = xml_item->value();
					}*/
			}

			auto rc = systemSound->Register(path, name, type, true);
			if (rc == 0)
			{
				//something
			}
		}
	}
}
void LevelFactory::GetLights(rapidxml::xml_node<char>* root, Scene* scene)
{
	rapidxml::xml_node<char>* xmlLights = root->first_node("Lights");
	if (xmlLights)
	{
		//unsigned int debug_light_ID = 0;
		for (rapidxml::xml_node<>* xmlLight = xmlLights->first_node("Light");
			xmlLight;
			xmlLight = xmlLight->next_sibling())
		{
			ILight* light = nullptr;

			//Type
			{
				rapidxml::xml_attribute<char>* xml_constAtt = xmlLight->first_attribute("Type");
				if (xml_constAtt)
				{
					std::string lightType = xml_constAtt->value();
					if (lightType == "LightPoint")
					{
						light = new LightPoint();

						//Position						
						{
							rapidxml::xml_node<char>* xml_Position = xmlLight->first_node("Position");
							if (xml_Position)
							{
								Vector3D position = Vector3D();
								rapidxml::xml_attribute<char>* xml_x = xml_Position->first_attribute("X");
								if (xml_x)
								{
									position.x = (float)atof(xml_x->value());
								}
								rapidxml::xml_attribute<char>* xml_y = xml_Position->first_attribute("Y");
								if (xml_y)
								{
									position.y = (float)atof(xml_y->value());
								}
								rapidxml::xml_attribute<char>* xml_z = xml_Position->first_attribute("Z");
								if (xml_z)
								{
									position.z = (float)atof(xml_z->value());
								}	
								((LightPoint*)light)->SetPosition(position);
							}
						}					

						//Constant Attenuation
						{
							rapidxml::xml_node<char>* xml_constAtt = xmlLight->first_node("ConstAttenuation");
							if (xml_constAtt)
							{
								((LightPoint*)light)->SetConstantAttenuation(strtof(xml_constAtt->value(), NULL));
							}
						}

						//Constant Attenuation
						{
							rapidxml::xml_node<char>* xml_LinearAtt = xmlLight->first_node("LinearAttenuation");
							if (xml_LinearAtt)
							{
								((LightPoint*)light)->SetLinearAttenuation(strtof(xml_LinearAtt->value(), NULL));
							}
						}

						//Quadratic Attenuation
						{
							rapidxml::xml_node<char>* xml_QuadAtt = xmlLight->first_node("QuadraticAttenuation");
							if (xml_QuadAtt)
							{
								((LightPoint*)light)->SetQuadraticAttenuation(strtof(xml_QuadAtt->value(), NULL));
							}
						}

						//DistanceCutOff
						{
							rapidxml::xml_node<char>* xml_Item = xmlLight->first_node("DistanceCutOff");
							if (xml_Item)
							{
								((LightPoint*)light)->SetCutoffDistance(strtof(xml_Item->value(), NULL));
							}
						}

						//Collider
						{
							ParameterContainer params;
							params.Add(Parameter("string", "Shape", "Sphere", false));
							params.Add(Parameter("float", "radius", std::to_string(2.0f), false));
							params.Add(Parameter("float", "center", StringHelpers::Vec3ToStr(Vector3D(0.0f, 0.0f, 0.0f)), false));

							light->SetCollider(systemPhysics->GetColliderFactory()->Create(params));
						}
					}
					else if (lightType == "LightSpot")
					{
						light = new LightSpot();

						//Position
						Vector3D position = Vector3D();
						{
							rapidxml::xml_node<char>* xml_Position = xmlLight->first_node("Position");
							if (xml_Position)
							{
								rapidxml::xml_attribute<char>* xml_x = xml_Position->first_attribute("X");
								if (xml_x)
								{
									position.x = (float)atof(xml_x->value());
								}
								rapidxml::xml_attribute<char>* xml_y = xml_Position->first_attribute("Y");
								if (xml_y)
								{
									position.y = (float)atof(xml_y->value());
								}
								rapidxml::xml_attribute<char>* xml_z = xml_Position->first_attribute("Z");
								if (xml_z)
								{
									position.z = (float)atof(xml_z->value());
								}
							}
						}
						((LightSpot*)light)->SetPosition(position);

						//Constant Attenuation
						{
							rapidxml::xml_node<char>* xml_constAtt = xmlLight->first_node("ConstAttenuation");
							if (xml_constAtt)
							{
								((LightSpot*)light)->SetConstantAttenuation(strtof(xml_constAtt->value(), NULL));
							}
						}

						//Constant Attenuation
						{
							rapidxml::xml_node<char>* xml_LinearAtt = xmlLight->first_node("LinearAttenuation");
							if (xml_LinearAtt)
							{
								((LightSpot*)light)->SetLinearAttenuation(strtof(xml_LinearAtt->value(), NULL));
							}
						}

						//Quadratic Attenuation
						{
							rapidxml::xml_node<char>* xml_QuadAtt = xmlLight->first_node("QuadraticAttenuation");
							if (xml_QuadAtt)
							{
								((LightSpot*)light)->SetQuadraticAttenuation(strtof(xml_QuadAtt->value(), NULL));
							}
						}

						//Direction
						{
							rapidxml::xml_node<char>* xml_Item = xmlLight->first_node("Direction");
							if (xml_Item)
							{
								Vector3D direction = Vector3D();
								rapidxml::xml_attribute<char>* xml_x = xml_Item->first_attribute("X");
								if (xml_x)
								{
									direction.x = strtof(xml_x->value(), NULL);
								}
								rapidxml::xml_attribute<char>* xml_y = xml_Item->first_attribute("Y");
								if (xml_y)
								{
									direction.y = strtof(xml_y->value(), NULL);
								}
								rapidxml::xml_attribute<char>* xml_z = xml_Item->first_attribute("Z");
								if (xml_z)
								{
									direction.z = strtof(xml_z->value(), NULL);
								}

								((LightSpot*)light)->SetDirection(direction);
							}
						}

						//Inner Angle
						{
							rapidxml::xml_node<char>* xml_Item = xmlLight->first_node("InnerAngle");
							if (xml_Item)
							{
								((LightSpot*)light)->SetInnerAngle(strtof(xml_Item->value(), NULL));
							}
						}

						//Outer Angle
						{
							rapidxml::xml_node<char>* xml_Item = xmlLight->first_node("OuterAngle");
							if (xml_Item)
							{
								((LightSpot*)light)->SetOuterAngle(strtof(xml_Item->value(), NULL));
							}
						}

						//DistanceCutOff
						{
							rapidxml::xml_node<char>* xml_Item = xmlLight->first_node("DistanceCutOff");
							if (xml_Item)
							{
								((LightSpot*)light)->SetCutoffDistance(strtof(xml_Item->value(), NULL));
							}
						}

						//Collider
						{
							ParameterContainer params;
							params.Add(Parameter("string", "Shape", "Sphere", false));
							params.Add(Parameter("float", "radius", std::to_string(2.0f), false));
							params.Add(Parameter("float", "center", StringHelpers::Vec3ToStr(Vector3D(0.0f, 0.0f, 0.0f)), false));

							light->SetCollider(systemPhysics->GetColliderFactory()->Create(params));
						}
					}
					else if (lightType == "LightDirectional")
					{
						light = new LightDirectional();

						//Direction
						{
							rapidxml::xml_node<char>* xml_Item = xmlLight->first_node("Direction");
							if (xml_Item)
							{
								Vector3D direction = Vector3D();
								rapidxml::xml_attribute<char>* xml_x = xml_Item->first_attribute("X");
								if (xml_x)
								{
									direction.x = (float)atof(xml_x->value());
								}
								rapidxml::xml_attribute<char>* xml_y = xml_Item->first_attribute("Y");
								if (xml_y)
								{
									direction.y = (float)atof(xml_y->value());
								}
								rapidxml::xml_attribute<char>* xml_z = xml_Item->first_attribute("Z");
								if (xml_z)
								{
									direction.z = (float)atof(xml_z->value());
								}

								((LightDirectional*)light)->SetDirection(direction);
							}
						}

						//Collider
						{
							ParameterContainer params;
							params.Add(Parameter("string", "Shape", "Sphere", false));
							params.Add(Parameter("float", "radius", std::to_string(2.0f), false));
							params.Add(Parameter("float", "center", StringHelpers::Vec3ToStr(Vector3D(0.0f, 0.0f, 0.0f)), false));

							light->SetCollider(systemPhysics->GetColliderFactory()->Create(params));
						}
					}
					else
					{
						//Unknown
						continue;
					}
				}
			}

			if (light)
			{
				//Name
				{
					rapidxml::xml_attribute<char>* xml_Name = xmlLight->first_attribute("Name");
					if (xml_Name)
					{
						light->SetName(xml_Name->value());
					}
				}

				//DiffuseColor
				{
					rapidxml::xml_node<char>* xml_Color = xmlLight->first_node("Diffuse");
					if (xml_Color)
					{
						Vector4D diffuse = Vector4D();
						rapidxml::xml_attribute<char>* xml_r = xml_Color->first_attribute("R");
						if (xml_r)
						{
							diffuse.r = (float)atof(xml_r->value());
						}
						rapidxml::xml_attribute<char>* xml_g = xml_Color->first_attribute("G");
						if (xml_g)
						{
							diffuse.g = (float)atof(xml_g->value());
						}
						rapidxml::xml_attribute<char>* xml_b = xml_Color->first_attribute("B");
						if (xml_b)
						{
							diffuse.b = (float)atof(xml_b->value());
						}
						rapidxml::xml_attribute<char>* xml_a = xml_Color->first_attribute("A");
						if (xml_a)
						{
							diffuse.a = (float)atof(xml_a->value());
						}

						light->SetDiffuse(diffuse);
					}
				}

				//Specular
				{
					rapidxml::xml_node<char>* xml_Color = xmlLight->first_node("Specular");
					if (xml_Color)
					{
						Vector4D specular = Vector4D();
						rapidxml::xml_attribute<char>* xml_r = xml_Color->first_attribute("R");
						if (xml_r)
						{
							specular.r = (float)atof(xml_r->value());
						}
						rapidxml::xml_attribute<char>* xml_g = xml_Color->first_attribute("G");
						if (xml_g)
						{
							specular.g = (float)atof(xml_g->value());
						}
						rapidxml::xml_attribute<char>* xml_b = xml_Color->first_attribute("B");
						if (xml_b)
						{
							specular.b = (float)atof(xml_b->value());
						}
						rapidxml::xml_attribute<char>* xml_a = xml_Color->first_attribute("W");
						if (xml_a)
						{
							specular.a = (float)atof(xml_a->value());
						}

						light->SetSpecular(specular);
					}
				}

				//isVisible
				{
					rapidxml::xml_node<char>* xml_constAtt = xmlLight->first_node("IsVisible");
					if (xml_constAtt)
					{
						light->SetVisibility(strtoul(xml_constAtt->value(), NULL, 0) != 0);
					}
				}

				scene->Append(light);
			}
		}
	}
}
void LevelFactory::GetEntities(rapidxml::xml_node<char>* root, Scene* scene)
{
	rapidxml::xml_node<char>* xml_GameObjects = root->first_node("GameObjects");
	if (xml_GameObjects)
	{
		for (rapidxml::xml_node<>* xmlGameObject = xml_GameObjects->first_node("GameObject");
			xmlGameObject;
			xmlGameObject = xmlGameObject->next_sibling())
		{
			Entity* go = CreateEntity(xmlGameObject, scene);
			//Adds GameObject
			scene->Append(go);
		}
	}
}
void LevelFactory::GetAnimations(rapidxml::xml_node<char>* root, Scene* scene)
{
	//return;

	rapidxml::xml_node<char>* xmlAnimations = root->first_node("Animations");
	if (xmlAnimations)
	{
		for (rapidxml::xml_node<>* xmlAnimation = xmlAnimations->first_node("Animation");
			xmlAnimation;
			xmlAnimation = xmlAnimation->next_sibling())
		{
			ParameterContainer parameters;

			//Name
			{
				auto xml_Item = xmlAnimation->first_attribute("Name");
				if (xml_Item)
				{
					parameters.Add("std::string", "AnimationName", std::string(xml_Item->value()));
				}
			}

			//Type
			{
				auto xml_Item = xmlAnimation->first_attribute("Type");
				if (xml_Item)
				{
					parameters.Add("std::string", "Type", std::string(xml_Item->value()));
				}
			}

			//AnimationType
			{
				auto xml_Item = xmlAnimation->first_attribute("AnimationType");
				if (xml_Item)
				{
					parameters.Add("std::string", "AnimationType", std::string(xml_Item->value()));
				}
			}

			//AnimationTrigger
			{
				auto xml_Item = xmlAnimation->first_attribute("Trigger");
				if (xml_Item)
				{
					parameters.Add("std::string", "AnimationType", std::string(xml_Item->value()));
				}
			}

			//Delay
			{
				rapidxml::xml_node<char>* xml_Item = xmlAnimation->first_node("Delay");
				if (xml_Item)
				{
					parameters.Add("float", "Delay", std::string(xml_Item->value()));
				}
			}

			IScriptedAnimation* animation = systemScriptingLua->GetScriptedAnimationFactory()->Create(parameters);
			if (!animation) continue;

			parameters.Clear();

			//Scripts
			{
				rapidxml::xml_node<char>* xmlScripts = xmlAnimation->first_node("Scripts");
				if (xmlScripts)
				{
					for (rapidxml::xml_node<>* xmlScript = xmlScripts->first_node("Script");
						xmlScript;
						xmlScript = xmlScript->next_sibling())
					{
						//Type
						{
							auto xml_Item = xmlScript->first_attribute("Type");
							if (xml_Item)
							{
								parameters.Add("std::string", "Type", std::string(xml_Item->value()));
							}
						}

						//Path
						{
							rapidxml::xml_attribute<char>* xmlItem = xmlScript->first_attribute("Path");
							if (xmlItem)
							{
								parameters.Add("std::string", "Path", EngineFolders::GetScriptsPath() + xmlItem->value());
							}
						}

						//Parameters
						{
							rapidxml::xml_node<char>* xmlParameters = xmlScript->first_node("Parameters");
							if (xmlParameters)
							{
								for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
									xmlParameter;
									xmlParameter = xmlParameter->next_sibling())
								{
									std::string paramType;
									rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
									if (xml_ParamType)
									{
										paramType = xml_ParamType->value();
									}

									std::string paramName = xmlParameter->name();
									std::string paramValue = xmlParameter->value();

									parameters.Add(paramType, paramName, paramValue);
								}
							}
						}

						IScript* script = systemScriptingLua->GetScriptFactory()->Create(parameters);
						if (script)
						{
							animation->AddScript(script);
						}

						parameters.Clear();
					}
				}
			}

			scene->Append(animation);			
		}
	}
}
void LevelFactory::GetScenes(rapidxml::xml_node<char>* root, Level* level)
{
	rapidxml::xml_node<char>* xmlScenes = root->first_node("Scenes");
	if (xmlScenes)
	{
		for (rapidxml::xml_node<>* xmlScene = xmlScenes->first_node("Scene"); xmlScene; xmlScene = xmlScene->next_sibling())
		{
			Scene* scene = new Scene();

			//Name
			{
				rapidxml::xml_attribute<>* xml_Name = xmlScene->first_attribute("Name");
				if (xml_Name)
				{
					scene->SetName(xml_Name->value());
				}
			}

			//Should draw ~ tells the Video System that this scene can be drawn
			/*Default value is TRUE*/
			{
				rapidxml::xml_attribute<char>* xmlItem = xmlScene->first_attribute("ShouldDraw");
				if (xmlItem)
				{
					scene->SetShouldDraw(atoi(xmlItem->value()) != 0);
				}
			}

			//World properties
			{
				GetWorldProperties(xmlScene, scene);
			}

			//GameObjects
			{
				//GetGameObjects(xmlScene, scene);
				GetEntities(xmlScene, scene);
			}

			//Animations
			{
				GetAnimations(xmlScene, scene);
			}

			//Lights
			{
				GetLights(xmlScene, scene);
			}

			//Sounds
			{
				GetSounds(xmlScene, scene);
			}

			//Camera
			{
				GetCamera(xmlScene, scene);
			}

			//Framebuffer references
			{
				GetFramebufferReferences(xmlScene, scene);
			}

			//AI
			{
				GetAI(xmlScene, scene);
			}

			level->Append(scene);
		}
	}
}
void LevelFactory::GetWorldProperties(rapidxml::xml_node<char>* root, Scene* scene)
{
	rapidxml::xml_node<char>* xmlWorld = root->first_node("World");
	if (xmlWorld)
	{
		rapidxml::xml_node<char>* xmlGravity = xmlWorld->first_node("Gravity");
		if (xmlGravity)
		{
			Vector3D _gravity = Vector3D();
			rapidxml::xml_attribute<char>* xml_x = xmlGravity->first_attribute("X");
			if (xml_x)
			{
				_gravity.x = (float)atof(xml_x->value());
			}
			rapidxml::xml_attribute<char>* xml_y = xmlGravity->first_attribute("Y");
			if (xml_y)
			{
				_gravity.y = (float)atof(xml_y->value());
			}
			rapidxml::xml_attribute<char>* xml_z = xmlGravity->first_attribute("Z");
			if (xml_z)
			{
				_gravity.z = (float)atof(xml_z->value());
			}

			scene->SetGravity(_gravity);
		}

		//AirFriction
		{
			rapidxml::xml_node<>* xml_Item = xmlWorld->first_node("AirFriction");
			if (xml_Item)
			{
				scene->SetAirFriction(atof(xml_Item->value()));
			}
		}
	}
}
void LevelFactory::GetFrameBuffers(rapidxml::xml_node<char>* root, Level* level)
{
	IFactory<IGraphicsBuffer>* frameBufferFactory = new GraphicsBufferFactory();

	rapidxml::xml_node<char>* xmlFBOs = root->first_node("FrameBuffers");
	if (xmlFBOs)
	{
		for (rapidxml::xml_node<>* xmlFBO = xmlFBOs->first_node("FrameBuffer"); xmlFBO; xmlFBO = xmlFBO->next_sibling())
		{
			ParameterContainer params;

			//FBO name
			{
				auto xmlItem = xmlFBO->first_attribute("Name");
				if (xmlItem)
				{
					params.Add(Parameter("string", "Name", std::string(xmlItem->value()), false));
				}
			}

			//Type
			{
				auto xmlItem = xmlFBO->first_attribute("Type");
				if (xmlItem)
				{
					params.Add(Parameter("string", "Type", std::string(xmlItem->value()), false));
				}
			}

			//Parent
			{
				auto xmlItem = xmlFBO->first_node("Parent");
				if (xmlItem)
				{
					params.Add(Parameter("string", "Parent", std::string(xmlItem->value()), false));
				}
			}

			//CameraReference
			{
				auto xmlItem = xmlFBO->first_node("Camera");
				if (xmlItem)
				{
					params.Add(Parameter("string", "Camera", std::string(xmlItem->value()), false));
				}
			}

			//Object
			{
				auto xmlItem = xmlFBO->first_node("Target");
				if (xmlItem)
				{
					params.Add(Parameter("string", "Target", std::string(xmlItem->value()), false));
				}
			}

			//Passes
			{
				auto xmlItem = xmlFBO->first_node("ShaderPasses");
				rapidxml::xml_node<char>* xmlParameters = xmlItem->first_node("Parameters");
				if (xmlParameters)
				{
					for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
						xmlParameter;
						xmlParameter = xmlParameter->next_sibling())
					{
						std::string paramType;
						rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
						if (xml_ParamType)
						{
							paramType = xml_ParamType->value();
						}

						bool variable = false;
						rapidxml::xml_attribute<char>* xml_ParamVariable = xmlParameter->first_attribute("Variable");
						if (xml_ParamVariable)
						{
							variable = xml_ParamVariable->value() != "0";
						}

						std::string paramName = xmlParameter->name();
						std::string paramValue = xmlParameter->value();

						params.Add(Parameter(paramType, paramName, paramValue, false));
					}
				}
			}

			IGraphicsBuffer* frameBuffer = frameBufferFactory->Create(params);
			bool rc = level->Append(frameBuffer);
			if (!rc)
			{
				//U wot m8?
			}
		}
	}

	delete (frameBufferFactory);
}
void LevelFactory::GetAI(rapidxml::xml_node<char>* root, Scene* scene)
{
	rapidxml::xml_node<char>* xmlAIGroupBehaviors = root->first_node("AIGroupBehaviors");
	if (xmlAIGroupBehaviors)
	{
		for (rapidxml::xml_node<>* xmlAIBehavior = xmlAIGroupBehaviors->first_node("AIBehavior"); xmlAIBehavior; xmlAIBehavior = xmlAIBehavior->next_sibling())
		{
			//Parameters
			ParameterContainer parameterContainer;
			//Type					
			{
				auto xmlItem = xmlAIBehavior->first_attribute("Type");
				if (xmlItem)
				{
					std::string type = xmlItem->value();
					parameterContainer.Add(Parameter("string", "Type", type, false));
				}
			}


			{
				rapidxml::xml_node<char>* xmlParameters = xmlAIBehavior->first_node("Parameters");
				if (xmlParameters)
				{
					for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
						xmlParameter;
						xmlParameter = xmlParameter->next_sibling())
					{
						std::string paramType;
						rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
						if (xml_ParamType)
						{
							paramType = xml_ParamType->value();
						}

						bool variable = false;
						rapidxml::xml_attribute<char>* xml_ParamVariable = xmlParameter->first_attribute("Variable");
						if (xml_ParamVariable)
						{
							variable = xml_ParamVariable->value() != "0";
						}

						std::string paramName = xmlParameter->name();
						std::string paramValue = xmlParameter->value();
						void* valuePtr = nullptr;

						if (paramType == "GameObject")
						{
							valuePtr = scene->FindEntityByName(paramValue);
						}
						else
						{
							//Treat value as string
						}

						if (valuePtr)
						{
							parameterContainer.Add(Parameter(paramType, paramName, valuePtr, variable));
						}
						else
						{
							parameterContainer.Add(Parameter(paramType, paramName, paramValue, variable));
						}
					}
				}
			}

			auto ai = systemAI->GetBehaviorFactory()->Create(parameterContainer);
			if (ai)
			{
				scene->Append(ai);
			}
		}
	}
}
Entity* LevelFactory::CreateEntity(rapidxml::xml_node<char>* root, Scene* scene)
{
	Entity* entity = new Entity();

	//Type
	std::string goType;
	{
		auto goTypeXML = root->first_attribute("Type");
		if (goTypeXML)
		{
			goType = goTypeXML->value();	

			if (goType == "GameObject")
			{
				entity = new Entity();
				entity->SetTypeStr(goType);
				entity->SetTypeID(ENGINE_OBJ_ID_SCN_GAMEOBJECT);
				entity->AddParent(scene);
			}
			else if (goType == "Skybox")
			{
				entity = new Entity();
				entity->SetTypeStr(goType);
				entity->SetTypeID(ENGINE_OBJ_ID_SCN_SKYBOX);
				entity->AddParent(scene);
			}
			else if (goType == "Cloth")
			{
				entity = new Cloth();
				entity->SetTypeStr(goType);
				entity->SetTypeID(ENGINE_OBJ_ID_SCN_CLOTH);
				entity->AddParent(scene);
			}
			else
			{
				//Unknown GameObject
				delete(entity);
				entity = nullptr;

				return nullptr;
			}
		}
		else
		{
			//Unknown GameObject
			return nullptr;
		}
	}

	//Name
	{
		auto nameXML = root->first_attribute("Name");
		if (nameXML)
		{
			entity->SetName(nameXML->value());
		}
	}

	//Mesh
	{
		/*auto xmlItem = xmlGameObject->first_attribute("Mesh");
		if (xmlItem)
		{
			std::string filepath = EngineFolders::GetModelsPath() + xmlItem->value();

			if (systemMesh->RegisterMesh(filepath))
			{
				if (!systemMesh->FindMeshByPath(filepath, go->meshes))
				{
					printf("Could not find meshes for path %s\n", filepath.c_str());
				}
			}
			else
			{
				printf("Could not register meshes for path %s\n", filepath.c_str());
			}
		}
		else
		{
			printf("No mesh file declared in mesh node for object %s.\n", go->GetName().c_str());
		}*/
	}

	//Components
	{
		rapidxml::xml_node<char>* xmlComponents = root->first_node("Components");
		if (xmlComponents)
		{
			rapidxml::xml_node<char>* xmlMeshes = xmlComponents->first_node("MeshComponents");
			if (xmlMeshes)
			{
				for (rapidxml::xml_node<>* xmlMeshComponent = xmlMeshes->first_node("MeshComponent"); xmlMeshComponent; xmlMeshComponent = xmlMeshComponent->next_sibling())
				{
					//Parameters
					ParameterContainer parameterContainer;

					//Type	
					std::string type;
					{
						auto xmlItem = xmlMeshComponent->first_attribute("Type");
						if (xmlItem)
						{
							type = xmlItem->value();
							parameterContainer.Add(Parameter("string", "Type", type, false));
						}
					}


					{
						rapidxml::xml_node<char>* xmlParameters = xmlMeshComponent->first_node("Parameters");
						if (xmlParameters)
						{
							for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
								xmlParameter;
								xmlParameter = xmlParameter->next_sibling())
							{
								std::string paramType;
								rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
								if (xml_ParamType)
								{
									paramType = xml_ParamType->value();
								}

								bool variable = false;
								rapidxml::xml_attribute<char>* xml_ParamVariable = xmlParameter->first_attribute("Variable");
								if (xml_ParamVariable)
								{
									variable = xml_ParamVariable->value() != "0";
								}

								std::string paramName = xmlParameter->name();
								std::string paramValue = xmlParameter->value();

								parameterContainer.Add(Parameter(paramType, paramName, paramValue, variable));
							}
						}
					}

					if (type == "MeshComponent")
					{
						IComponent* c = systemMesh->GetMeshComponentFactory()->Create(parameterContainer);
						if (!entity->AddComponent(c))
						{
							printf("LevelFactory: %s\n", "Could not load MeshComponent");
						}
					}
					else if (type == "ParticleMeshComponent")
					{
						IComponent* c = systemMesh->GetParticleMeshComponentFactory()->Create(parameterContainer);
						if (!entity->AddComponent(c))
						{
							printf("LevelFactory: %s\n", "Could not load PArticleMeshComponent");
						}
					}
					else 
					{
						//Unknown
					}
				}	
			}

			rapidxml::xml_node<char>* xmlAIBehaviors = xmlComponents->first_node("AIBehaviors");
			if (xmlAIBehaviors)
			{
				for (rapidxml::xml_node<>* xmlAIBehavior = xmlAIBehaviors->first_node("AIBehavior"); xmlAIBehavior; xmlAIBehavior = xmlAIBehavior->next_sibling())
				{
					//Parameters
					ParameterContainer parameterContainer;

					//Type	
					{
						auto xmlItem = xmlAIBehavior->first_attribute("Type");
						if (xmlItem)
						{
							std::string type = xmlItem->value();
							parameterContainer.Add(Parameter("string", "Type", type, false));
						}
					}


					{
						rapidxml::xml_node<char>* xmlParameters = xmlAIBehavior->first_node("Parameters");
						if (xmlParameters)
						{
							unsigned long pointersCount = 0;
							for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
								xmlParameter;
								xmlParameter = xmlParameter->next_sibling())
							{
								std::string paramType;
								rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
								if (xml_ParamType)
								{
									paramType = xml_ParamType->value();
								}

								bool variable = false;
								rapidxml::xml_attribute<char>* xml_ParamVariable = xmlParameter->first_attribute("Variable");
								if (xml_ParamVariable)
								{
									variable = xml_ParamVariable->value() != "0";
								}

								std::string paramName = xmlParameter->name();
								std::string paramValue = xmlParameter->value();
								void* valuePtr = nullptr;

								if (paramName == "Target" || paramName == "Projectile")
								{
									valuePtr = scene->FindEntityByName(paramValue);
								}
								else
								{
									//Treat value as string
								}

								if (valuePtr)
								{
									parameterContainer.Add(Parameter(paramType, paramName, valuePtr, variable));
								}
								else
								{
									parameterContainer.Add(Parameter(paramType, paramName, paramValue, variable));
								}
							}
						}
					}

					parameterContainer.Add(Parameter("GameObject", "Parent", entity, false));
					IAIBehavior* ai = systemAI->GetBehaviorFactory()->Create(parameterContainer);
					entity->AddComponent(ai);
				}
			}

			rapidxml::xml_node<char>* xmlSkinnedMeshes = xmlComponents->first_node("SkinnedMeshes");
			if (xmlSkinnedMeshes)
			{
				for (rapidxml::xml_node<>* xmlSkinnedMesh = xmlSkinnedMeshes->first_node("SkinnedMesh"); xmlSkinnedMesh; xmlSkinnedMesh = xmlSkinnedMesh->next_sibling())
				{
					//Parameters
					ParameterContainer parameterContainer;

					//Type	
					{
						auto xmlItem = xmlSkinnedMesh->first_attribute("Type");
						if (xmlItem)
						{
							std::string type = xmlItem->value();
							parameterContainer.Add(Parameter("string", "Type", type, false));
						}
					}


					{
						rapidxml::xml_node<char>* xmlParameters = xmlSkinnedMesh->first_node("Parameters");
						if (xmlParameters)
						{
							for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
								xmlParameter;
								xmlParameter = xmlParameter->next_sibling())
							{
								std::string paramType;
								rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
								if (xml_ParamType)
								{
									paramType = xml_ParamType->value();
								}

								bool variable = false;
								rapidxml::xml_attribute<char>* xml_ParamVariable = xmlParameter->first_attribute("Variable");
								if (xml_ParamVariable)
								{
									variable = xml_ParamVariable->value() != "0";
								}

								std::string paramName = xmlParameter->name();
								std::string paramValue = xmlParameter->value();

								parameterContainer.Add(Parameter(paramType, paramName, paramValue, variable));
							}
						}
					}

					IComponent* skm = componentFactory->Create(parameterContainer);
					if (!entity->AddComponent(skm))
					{
						printf("LevelFactory: %s\n", componentFactory->GetLastErrorString().c_str());
					}
				}

			}

			rapidxml::xml_node<char>* xmlRenderComponents = xmlComponents->first_node("RenderComponents");
			if (xmlRenderComponents)
			{
				for (rapidxml::xml_node<>* xmlRenderComponent = xmlRenderComponents->first_node("RenderComponent"); xmlRenderComponent; xmlRenderComponent = xmlRenderComponent->next_sibling())
				{
					//Parameters
					ParameterContainer parameterContainer;

					//Type	
					{
						auto xmlItem = xmlRenderComponent->first_attribute("Type");
						if (xmlItem)
						{
							std::string type = xmlItem->value();
							parameterContainer.Add(Parameter("string", "Type", type, false));
						}
					}

					//Parameters
					{
						rapidxml::xml_node<char>* xmlParameters = xmlRenderComponent->first_node("Parameters");
						if (xmlParameters)
						{
							for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
								xmlParameter;
								xmlParameter = xmlParameter->next_sibling())
							{
								std::string paramType;
								rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
								if (xml_ParamType)
								{
									paramType = xml_ParamType->value();
								}

								bool variable = false;
								rapidxml::xml_attribute<char>* xml_ParamVariable = xmlParameter->first_attribute("Variable");
								if (xml_ParamVariable)
								{
									variable = xml_ParamVariable->value() != "0";
								}

								std::string paramName = xmlParameter->name();
								std::string paramValue = xmlParameter->value();

								parameterContainer.Add(Parameter(paramType, paramName, paramValue, variable));
							}
						}
					}

					IComponent* vr = videoFactory->Create(parameterContainer);
					if (vr->GetTypeID() == ENGINE_OBJ_ID_GPH_RENDERCOMPONENT || 
						vr->GetTypeID() == ENGINE_OBJ_ID_GPH_CLOTHRENDERCOMPONENT)
					{
						//ShaderFeatures
						std::vector<ShaderFeature*> shaderFeatures;
						{
							rapidxml::xml_node<char>* xmlSFs = xmlRenderComponent->first_node("ShaderFeatures");
							if (xmlSFs)
							{
								for (rapidxml::xml_node<>* xmlSF = xmlSFs->first_node("ShaderFeature");
									xmlSF;
									xmlSF = xmlSF->next_sibling())
								{
									ShaderFeature* shaderFeature = new ShaderFeature();

									//Name
									std::string name;
									{
										rapidxml::xml_attribute<char>* xmlItem = xmlSF->first_attribute("Name");
										if (xmlItem)
										{
											name = xmlItem->value();
										}
									}
									shaderFeature->SetName(name);

									//ShaderType
									std::string shaderType;
									{
										rapidxml::xml_attribute<char>* xmlItem = xmlSF->first_attribute("ShaderType");
										if (xmlItem)
										{
											shaderType = xmlItem->value();
										}
									}
									shaderFeature->SetShaderType(shaderType);

									//IsActive or enabled
									bool isActive = true;
									{
										rapidxml::xml_attribute<char>* xmlItem = xmlSF->first_attribute("IsActive");
										if (xmlItem)
										{
											isActive = strtoul(xmlItem->value(), NULL, 0) != 0;
										}
									}
									shaderFeature->Activate(isActive);

									//Parameters
									{
										rapidxml::xml_node<char>* xmlParameters = xmlSF->first_node("Parameters");
										if (xmlParameters)
										{
											for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
												xmlParameter;
												xmlParameter = xmlParameter->next_sibling())
											{
												std::string paramType;
												rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
												if (xml_ParamType)
												{
													paramType = xml_ParamType->value();
												}

												bool variable = false;
												rapidxml::xml_attribute<char>* xml_ParamVariable = xmlParameter->first_attribute("Variable");
												if (xml_ParamVariable)
												{
													variable = xml_ParamVariable->value() != "0";
												}

												std::string paramName = xmlParameter->name();
												std::string paramValue = xmlParameter->value();

												shaderFeature->parameters.Add(Parameter::Parameter(paramType, paramName, paramValue, variable));

												if (variable)
												{
													paramName = paramName + "_VAR";
													shaderFeature->parameters.Add(Parameter::Parameter(paramType, paramName, paramValue, variable));
												}
											}
										}
									}

									shaderFeatures.push_back(shaderFeature);
								}
							}
						}

						for (ShaderFeature* sf : shaderFeatures)
						{
							((IRender*)vr)->Append(sf);
						}
					}
					if (!entity->AddComponent(vr))
					{
						printf("LevelFactory: %s\n", componentFactory->GetLastErrorString().c_str());
					}
				}
			}

			rapidxml::xml_node<char>* xmlPhysicsComponents = xmlComponents->first_node("PhysicsComponents");
			if (xmlPhysicsComponents)
			{
				for (rapidxml::xml_node<>* xmlPhysicsComponent = xmlPhysicsComponents->first_node("PhysicsComponent"); xmlPhysicsComponent; xmlPhysicsComponent = xmlPhysicsComponent->next_sibling())
				{
					//Parameters
					ParameterContainer parameterContainer;

					//Type	
					{
						auto xmlItem = xmlPhysicsComponent->first_attribute("Type");
						if (xmlItem)
						{
							std::string type = xmlItem->value();
							parameterContainer.Add(Parameter("string", "Type", type, false));
						}
					}

					{
						rapidxml::xml_node<char>* xmlParameters = xmlPhysicsComponent->first_node("Parameters");
						if (xmlParameters)
						{
							for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
								xmlParameter;
								xmlParameter = xmlParameter->next_sibling())
							{
								std::string paramType;
								rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
								if (xml_ParamType)
								{
									paramType = xml_ParamType->value();
								}

								bool variable = false;
								rapidxml::xml_attribute<char>* xml_ParamVariable = xmlParameter->first_attribute("Variable");
								if (xml_ParamVariable)
								{
									variable = xml_ParamVariable->value() != "0";
								}

								std::string paramName = xmlParameter->name();
								std::string paramValue = xmlParameter->value();

								parameterContainer.Add(Parameter(paramType, paramName, paramValue, variable));
							}
						}
					}

					Parameter p;
					std::vector<IMesh*> meshes;
					if (parameterContainer.FindParameterByName("UseMesh", p) && atoi(p.value.c_str()) != 0)
					{
						IMeshComponent* mC = entity->GetComponent<IMeshComponent*>();
						if (mC)
						{
							if (mC->GetMeshes(meshes))
							{
								parameterContainer.Add(Parameter("std:vector<IMesh>", "mesh", &meshes, false));
							}
						}
					}

					parameterContainer.Add(Parameter("IGameObject", "parent", entity, false));

					IPhysicsComponent* pC = systemPhysics->GetPhysicsComponentFactory()->Create(parameterContainer);
					if (!pC || !entity->AddComponent(pC))
					{
						printf("LevelFactory: %s\n", componentFactory->GetLastErrorString().c_str());
					}
				}

			}

			rapidxml::xml_node<char>* xmlSoundComponents = xmlComponents->first_node("SoundComponents");
			if (xmlSoundComponents)
			{
				for (rapidxml::xml_node<>* xmlSoundComponent = xmlSoundComponents->first_node("SoundComponent");
					xmlSoundComponent; 
					xmlSoundComponent = xmlSoundComponent->next_sibling())
				{
					//Parameters
					ParameterContainer parameterContainer;

					//Type	
					auto xmlItem = xmlSoundComponent->first_attribute("Type");
					if (xmlItem)
					{
						std::string type = xmlItem->value();
						parameterContainer.Add("string", "Type", type);
					}

					rapidxml::xml_node<char>* xmlParameters = xmlSoundComponent->first_node("Parameters");
					if (xmlParameters)
					{
						for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
							xmlParameter;
							xmlParameter = xmlParameter->next_sibling())
						{
							std::string paramType;
							rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
							if (xml_ParamType) paramType = xml_ParamType->value();

							bool variable = false;
							rapidxml::xml_attribute<char>* xml_ParamVariable = xmlParameter->first_attribute("Variable");
							if (xml_ParamVariable)	variable = xml_ParamVariable->value() != "0";

							std::string paramName = xmlParameter->name();
							std::string paramValue = xmlParameter->value();
							parameterContainer.Add(paramType, paramName, paramValue);
						}
					}

					IComponent* c = systemSound->GetSoundComponentFactory()->Create(parameterContainer);
					if (!entity->AddComponent(c))
					{
						printf("LevelFactory: %s\n", "Could not create soundComponent.");
					}
				}
			}

			rapidxml::xml_node<char>* xmlParticleEmitterComponents = xmlComponents->first_node("ParticleEmitterComponents");
			if (xmlParticleEmitterComponents)
			{
				for (rapidxml::xml_node<>* xmlParticleEmitter = xmlParticleEmitterComponents->first_node("ParticleEmitterComponent");
					xmlParticleEmitter;
					xmlParticleEmitter = xmlParticleEmitter->next_sibling())
				{
					//Parameters
					ParameterContainer parameterContainer;

					//Type	
					auto xmlItem = xmlParticleEmitter->first_attribute("Type");
					if (xmlItem)
					{
						std::string type = xmlItem->value();
						parameterContainer.Add("string", "Type", type);
					}

					rapidxml::xml_node<char>* xmlParameters = xmlParticleEmitter->first_node("Parameters");
					if (xmlParameters)
					{
						for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
							xmlParameter;
							xmlParameter = xmlParameter->next_sibling())
						{
							std::string paramType;
							rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
							if (xml_ParamType) paramType = xml_ParamType->value();

							bool variable = false;
							rapidxml::xml_attribute<char>* xml_ParamVariable = xmlParameter->first_attribute("Variable");
							if (xml_ParamVariable)	variable = xml_ParamVariable->value() != "0";

							std::string paramName = xmlParameter->name();
							std::string paramValue = xmlParameter->value();
							parameterContainer.Add(paramType, paramName, paramValue);
						}
					}

					//ShaderFeatures
					std::vector<ShaderFeature*> shaderFeatures;
					{
						rapidxml::xml_node<char>* xmlSFs = xmlParticleEmitter->first_node("ShaderFeatures");
						if (xmlSFs)
						{
							for (rapidxml::xml_node<>* xmlSF = xmlSFs->first_node("ShaderFeature");
								xmlSF;
								xmlSF = xmlSF->next_sibling())
							{
								ShaderFeature* shaderFeature = new ShaderFeature();

								//Name
								std::string name;
								{
									rapidxml::xml_attribute<char>* xmlItem = xmlSF->first_attribute("Name");
									if (xmlItem)
									{
										name = xmlItem->value();
									}
								}
								shaderFeature->SetName(name);

								//ShaderType
								std::string shaderType;
								{
									rapidxml::xml_attribute<char>* xmlItem = xmlSF->first_attribute("ShaderType");
									if (xmlItem)
									{
										shaderType = xmlItem->value();
									}
								}
								shaderFeature->SetShaderType(shaderType);

								//Parameters
								{
									rapidxml::xml_node<char>* xmlParameters = xmlSF->first_node("Parameters");
									if (xmlParameters)
									{
										for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
											xmlParameter;
											xmlParameter = xmlParameter->next_sibling())
										{
											std::string paramType;
											rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
											if (xml_ParamType)
											{
												paramType = xml_ParamType->value();
											}

											bool variable = false;
											rapidxml::xml_attribute<char>* xml_ParamVariable = xmlParameter->first_attribute("Variable");
											if (xml_ParamVariable)
											{
												variable = xml_ParamVariable->value() != "0";
											}

											std::string paramName = xmlParameter->name();
											std::string paramValue = xmlParameter->value();

											shaderFeature->parameters.Add(Parameter::Parameter(paramType, paramName, paramValue, variable));

											if (variable)
											{
												paramName = paramName + "_VAR";
												shaderFeature->parameters.Add(Parameter::Parameter(paramType, paramName, paramValue, variable));
											}
										}
									}
								}

								shaderFeatures.push_back(shaderFeature);
							}
						}
					}

					IComponent* c = videoFactory->Create(parameterContainer);
					if (!entity->AddComponent(c))
					{
						printf("LevelFactory: %s\n", "Could not create ParticleEmitterComponent.");
					}

					for (ShaderFeature* sf : shaderFeatures)
					{
						((IParticleEmitter*)c)->AddShaderFeature(sf);
					}
				}
			}

			rapidxml::xml_node<char>* xmlLightComponents = xmlComponents->first_node("LightComponents");
			if (xmlLightComponents)
			{
				for (rapidxml::xml_node<>* xmlLightComponent = xmlLightComponents->first_node("LightComponent");
					xmlLightComponent;
					xmlLightComponent = xmlLightComponent->next_sibling())
				{
					//Parameters
					ParameterContainer parameterContainer;

					//Type	
					auto xmlItem = xmlLightComponent->first_attribute("Type");
					if (xmlItem)
					{
						std::string type = xmlItem->value();
						parameterContainer.Add("string", "Type", type);
					}

					rapidxml::xml_node<char>* xmlParameters = xmlLightComponent->first_node("Parameters");
					if (xmlParameters)
					{
						for (rapidxml::xml_node<>* xmlParameter = xmlParameters->first_node();
							xmlParameter;
							xmlParameter = xmlParameter->next_sibling())
						{
							std::string paramType;
							rapidxml::xml_attribute<char>* xml_ParamType = xmlParameter->first_attribute("Type");
							if (xml_ParamType) paramType = xml_ParamType->value();

							bool variable = false;
							rapidxml::xml_attribute<char>* xml_ParamVariable = xmlParameter->first_attribute("Variable");
							if (xml_ParamVariable)	variable = xml_ParamVariable->value() != "0";

							std::string paramName = xmlParameter->name();
							std::string paramValue = xmlParameter->value();
							parameterContainer.Add(paramType, paramName, paramValue);
						}
					}

					IComponent* c = videoFactory->Create(parameterContainer);
					if (c)
					{
						ParameterContainer params;
						params.Add(Parameter("string", "Shape", "Sphere", false));
						params.Add(Parameter("float", "radius", std::to_string(2.0f), false));
						params.Add(Parameter("float", "center", StringHelpers::Vec3ToStr(Vector3D(0.0f, 0.0f, 0.0f)), false));

						((ILight*)c)->SetCollider(systemPhysics->GetColliderFactory()->Create(params));

						if (!entity->AddComponent(c))
						{
							printf("LevelFactory: %s\n", "Could not create lightComponent.");
						}
					}					
				}
			}
		}
	}

	return entity;
}
std::vector<Level*> LevelFactory::GetLevels(const std::string& chapterDefinitionFile)
{
	std::vector<Level*> levels;

	rapidxml::file<> xmlFile(chapterDefinitionFile.c_str());
	rapidxml::xml_document<>* xml = new rapidxml::xml_document<>();
	if (xml)
	{
		xml->parse<0>(xmlFile.data());

		//find root
		rapidxml::xml_node<>* root = xml->first_node();

		for (rapidxml::xml_node<>* xmlLevel = root->first_node("Level"); xmlLevel; xmlLevel = xmlLevel->next_sibling())
		{
			//ID
			int id = -1;
			{
				auto xmlitem = xmlLevel->first_attribute("ID");
				if (xmlitem)
				{
					id = atoi(xmlitem->value());
				}
			}

			//Path
			std::string path = "";
			{
				auto xmlitem = xmlLevel->first_attribute("Path");
				if (xmlitem)
				{
					path = EngineFolders::GetConfigPath() + xmlitem->value();
				}
			}

			Level* level = GetLevel(path);
			level->id = id;
			levels.push_back(level);
		}
	}

	delete (xml);

	return levels;
}
bool LevelFactory::GetLevels(const std::string& chapterDefinitionFile, std::vector<std::string>& paths)
{
	bool rc = false;

#if __cplusplus == 201703L
	rc = std::filesystem::exists(chapterDefinitionFile);
	if (!rc)
	{
		SetLastErrorString("The path to this chapter definition does not exist.");
		return rc;
	}
#endif

	paths.clear();
	paths.shrink_to_fit();
	rapidxml::file<> xmlFile(chapterDefinitionFile.c_str());
	rapidxml::xml_document<>* xml = new rapidxml::xml_document<>();
	if (xml)
	{
		xml->parse<0>(xmlFile.data());

		//find root
		rapidxml::xml_node<>* root = xml->first_node();

		for (rapidxml::xml_node<>* xmlLevel = root->first_node("Level"); xmlLevel; xmlLevel = xmlLevel->next_sibling())
		{
			//ID
			int id = -1;
			{
				auto xmlitem = xmlLevel->first_attribute("ID");
				if (xmlitem)
				{
					id = atoi(xmlitem->value());
				}
			}

			//Path
			std::string path = "";
			{
				auto xmlitem = xmlLevel->first_attribute("Path");
				if (xmlitem)
				{
					path = EngineFolders::GetConfigPath() + xmlitem->value();
				}
			}

			//If the path is not null, return it
			if (!path.empty())
			{
				paths.push_back(path);
			}
		}
	}
	else
	{
		SetLastErrorString("XML object is invalid.");
	}

	rc = paths.size() != 0;

	delete (xml);

	return rc;
}
