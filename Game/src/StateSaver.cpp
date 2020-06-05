/***********************************************************************
* Project           : <ProjectName>
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT
* Comments:			: Midterm for INFO 6044 - Game Engine & Patterns
************************************************************************/
#include "StateSaver.h"
#include <GLCommon.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#pragma warning(push, 0)   
#include "rapidxml_ext.h"
#include <rapidxml_utils.hpp>
#pragma warning(pop)
#include "EngineFolders.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <CameraFree.h>
#include <CameraLockedAt.h>
#include <LightPoint.h>
#include <LightSpot.h>
#include <LightDirectional.h>
#include <IRender.h>

void StateSaver::WriteXMLHeader(std::string version, std::string encoding, rapidxml::xml_document<>* xml)
{
	rapidxml::xml_node<>* decl = xml->allocate_node(rapidxml::node_type::node_declaration);

	const char* _versionVal = xml->allocate_string(version.c_str());
	decl->append_attribute(xml->allocate_attribute("version", _versionVal));

	const char* _encodingVal = xml->allocate_string(encoding.c_str());
	decl->append_attribute(xml->allocate_attribute("encoding", _encodingVal));

	xml->append_node(decl);
}

void StateSaver::Serialize(Level* level, rapidxml::xml_document<>* xml)
{
	if (xml)
	{
		rapidxml::xml_node<>* root = xml->allocate_node(rapidxml::node_type::node_element, "Level");

		const char* _nameVal = xml->allocate_string(level->GetName().c_str());
		rapidxml::xml_attribute<char>* _key = xml->allocate_attribute("Name", _nameVal);
		root->append_attribute(_key);

		for (Scene* scene : *level->GetScenes())
		{
			rapidxml::xml_node<char>* sceneXML = xml->allocate_node(rapidxml::node_type::node_element, "Scene");

			//Name
			{
				const char* _nameVal = xml->allocate_string(scene->GetName().c_str());
				rapidxml::xml_attribute<char>* _key = xml->allocate_attribute("Name", _nameVal);
				sceneXML->append_attribute(_key);
			}		

			//Cameras
			{
				ICamera* camera = scene->GetCamera();

				if (camera)
				{
					std::string cameraType = camera->GetType();

					rapidxml::xml_node<char>* cameraXML = xml->allocate_node(rapidxml::node_type::node_element, "Camera");

					//Name
					{
						const char* name_val = xml->allocate_string(camera->GetName().c_str());
						rapidxml::xml_attribute<char>* _name_Attrib = xml->allocate_attribute("Name", name_val);
						cameraXML->append_attribute(_name_Attrib);
					}

					//Type
					{
						const char* type_val = xml->allocate_string(cameraType.c_str());
						rapidxml::xml_attribute<char>* _type_Attrib = xml->allocate_attribute("Type", type_val);
						cameraXML->append_attribute(_type_Attrib);
					}

					//EyePosition
					{
						Vector3D eyePosition = camera->GetEyePosition();

						rapidxml::xml_node<>* content = xml->allocate_node(rapidxml::node_type::node_element, "EyePosition");
						const char* _x_val = xml->allocate_string(std::to_string(eyePosition.x).c_str());
						rapidxml::xml_attribute<char>* _x_Attrib = xml->allocate_attribute("X", _x_val);
						content->append_attribute(_x_Attrib);

						const char* _y_val = xml->allocate_string(std::to_string(eyePosition.y).c_str());
						rapidxml::xml_attribute<char>* _y_Attrib = xml->allocate_attribute("Y", _y_val);
						content->append_attribute(_y_Attrib);

						const char* _z_val = xml->allocate_string(std::to_string(eyePosition.z).c_str());
						rapidxml::xml_attribute<char>* _z_Attrib = xml->allocate_attribute("Z", _z_val);
						content->append_attribute(_z_Attrib);

						cameraXML->append_node(content);
					}

					//FOV
					{
						rapidxml::xml_node<>* content = xml->allocate_node(rapidxml::node_type::node_element, "FOV");
						const char* inverseMass_val = xml->allocate_string(std::to_string(camera->GetFOV()).c_str());
						content->value(inverseMass_val);

						cameraXML->append_node(content);
					}

					//FarClippingPlane
					{
						rapidxml::xml_node<>* content = xml->allocate_node(rapidxml::node_type::node_element, "FarClippingPlane");
						const char* farClippingPlane_val = xml->allocate_string(std::to_string(camera->GetFarClippingPlane()).c_str());
						content->value(farClippingPlane_val);

						cameraXML->append_node(content);
					}

					//NearClippingPlane
					{
						rapidxml::xml_node<>* content = xml->allocate_node(rapidxml::node_type::node_element, "NearClippingPlane");
						const char* nearClippingPlane_val = xml->allocate_string(std::to_string(camera->GetNearClippingPlane()).c_str());
						content->value(nearClippingPlane_val);

						cameraXML->append_node(content);
					}

					//Properties according to types
					if (cameraType == "CameraLockedAt")
					{
						//Target
						{
							rapidxml::xml_node<>* content = xml->allocate_node(rapidxml::node_type::node_element, "Target");
							const char* value = xml->allocate_string(((CameraLockedAt*)camera)->GetTarget()->GetName().c_str());
							content->value(value);

							cameraXML->append_node(content);
						}
					}
					else if (cameraType == "CameraFree")
					{
						//Target
						{
							rapidxml::xml_node<>* content = xml->allocate_node(rapidxml::node_type::node_element, "Target");
							const char* _x_val = xml->allocate_string(std::to_string(camera->GetTargetPosition().x).c_str());
							rapidxml::xml_attribute<char>* _x_Attrib = xml->allocate_attribute("X", _x_val);

							const char* _y_val = xml->allocate_string(std::to_string(camera->GetTargetPosition().y).c_str());
							rapidxml::xml_attribute<char>* _y_Attrib = xml->allocate_attribute("Y", _y_val);

							const char* _z_val = xml->allocate_string(std::to_string(camera->GetTargetPosition().z).c_str());
							rapidxml::xml_attribute<char>* _z_Attrib = xml->allocate_attribute("Z", _z_val);

							content->append_attribute(_z_Attrib);
							content->append_attribute(_y_Attrib);
							content->append_attribute(_x_Attrib);

							cameraXML->append_node(content);
						}

						//Up
						{
							Vector3D up = camera->GetUp();

							rapidxml::xml_node<>* content = xml->allocate_node(rapidxml::node_type::node_element, "Up");
							const char* _x_val = xml->allocate_string(std::to_string(up.x).c_str());
							rapidxml::xml_attribute<char>* _x_Attrib = xml->allocate_attribute("X", _x_val);

							const char* _y_val = xml->allocate_string(std::to_string(up.y).c_str());
							rapidxml::xml_attribute<char>* _y_Attrib = xml->allocate_attribute("Y", _y_val);

							const char* _z_val = xml->allocate_string(std::to_string(up.z).c_str());
							rapidxml::xml_attribute<char>* _z_Attrib = xml->allocate_attribute("Z", _z_val);

							content->append_attribute(_z_Attrib);
							content->append_attribute(_y_Attrib);
							content->append_attribute(_x_Attrib);

							cameraXML->append_node(content);
						}
					}

					sceneXML->append_node(cameraXML);
				}
			}

			//Animations
			{
				//rapidxml::xml_node<>* xml_animations = xml->allocate_node(rapidxml::node_type::node_element, "Animations");

				//for (Animation* animation : *scene->GetAnimations())
				//{
				//	if (animation)
				//	{
				//		std::string animationType;
				//		if (animation->type == Animation::AnimationType_e::Serial)
				//		{
				//			animationType = "serial";
				//		}
				//		else if (animation->type == Animation::AnimationType_e::Parallel)
				//		{
				//			animationType = "parallel";
				//		}

				//		rapidxml::xml_node<char>* animationXML = xml->allocate_node(rapidxml::node_type::node_element, "Animation");

				//		//Name
				//		{
				//			const char* name_val = xml->allocate_string(animation->GetName().c_str());
				//			rapidxml::xml_attribute<char>* _name_Attrib = xml->allocate_attribute("Name", name_val);
				//			animationXML->append_attribute(_name_Attrib);
				//		}

				//		//Type
				//		{
				//			const char* type_val = xml->allocate_string(animationType.c_str());
				//			rapidxml::xml_attribute<char>* _type_Attrib = xml->allocate_attribute("Type", type_val);
				//			animationXML->append_attribute(_type_Attrib);
				//		}

				//		//Trigger
				//		{
				//			std::string triggerStr;
				//			if (animation->trigger == Animation::AnimationTrigger_e::Event)
				//			{
				//				triggerStr = "event";
				//			}
				//			else if (animation->trigger == Animation::AnimationTrigger_e::Immediate)
				//			{
				//				triggerStr = "immediate";
				//			}

				//			const char* type_val = xml->allocate_string(triggerStr.c_str());
				//			rapidxml::xml_attribute<char>* _type_Attrib = xml->allocate_attribute("Trigger", type_val);
				//			animationXML->append_attribute(_type_Attrib);
				//		}

				//		//Delay
				//		{
				//			rapidxml::xml_node<>* delay_XML = xml->allocate_node(rapidxml::node_type::node_element, "Delay");
				//			const char* delayVal = xml->allocate_string(std::to_string(animation->delay).c_str());
				//			delay_XML->value(delayVal);

				//			animationXML->append_node(delay_XML);
				//		}

				//		//Scripts
				//		{
				//			rapidxml::xml_node<>* scriptsXML = xml->allocate_node(rapidxml::node_type::node_element, "Scripts");

				//			for (unsigned int scriptIndex = 0; scriptIndex < animation->scripts.size(); scriptIndex++)
				//			{
				//				Script* scp = animation->scripts[scriptIndex];

				//				rapidxml::xml_node<>* scriptXML = xml->allocate_node(rapidxml::node_type::node_element, "Script");

				//				//Path
				//				{
				//					std::string pathOfScript = scp->GetPath().substr(scp->GetPath().find_last_of("/\\") + 1);
				//					const char* _nameVal = xml->allocate_string(pathOfScript.c_str());
				//					rapidxml::xml_attribute<char>* _nameAttrib = xml->allocate_attribute("Path", _nameVal);
				//					scriptXML->append_attribute(_nameAttrib);
				//				}

				//				//Parameters
				//				{
				//					rapidxml::xml_node<>* parameters = xml->allocate_node(rapidxml::node_type::node_element, "Parameters");

				//					for (std::pair<std::string, Parameter> par : scp->parameters)
				//					{
				//						//Name
				//						const char* _parName = xml->allocate_string(par.second.name.c_str());
				//						rapidxml::xml_node<>* parameter = xml->allocate_node(rapidxml::node_type::node_element, _parName);

				//						//Type
				//						{
				//							const char* _parType = xml->allocate_string(par.second.type.c_str());
				//							rapidxml::xml_attribute<char>* _typeAttrib = xml->allocate_attribute("Type", _parType);
				//							parameter->append_attribute(_typeAttrib);
				//						}

				//						//Value
				//						{
				//							const char* _parValue = xml->allocate_string(par.second.value.c_str());
				//							parameter->value(_parValue);
				//						}

				//						parameters->append_node(parameter);
				//					}

				//					scriptXML->append_node(parameters);
				//				}

				//				scriptsXML->append_node(scriptXML);
				//			}

				//			animationXML->append_node(scriptsXML);
				//		}

				//		xml_animations->append_node(animationXML);
				//	}
				//}

				//sceneXML->append_node(xml_animations);
			}

			//Lights
			{
				rapidxml::xml_node<>* lightsXML = xml->allocate_node(rapidxml::node_type::node_element, "Lights");

				for (ILight* light : *scene->GetLights())
				{
					if (light)
					{
						std::string lightType = light->GetType();

						rapidxml::xml_node<char>* lightXML = xml->allocate_node(rapidxml::node_type::node_element, "Light");

						//Name
						{
							const char* _nameVal = xml->allocate_string(light->GetName().c_str());
							rapidxml::xml_attribute<char>* _nameAttrib = xml->allocate_attribute("Name", _nameVal);
							lightXML->append_attribute(_nameAttrib);
						}

						//Type
						{
							const char* _typeVal = xml->allocate_string(lightType.c_str());
							rapidxml::xml_attribute<char>* _typeAttrib = xml->allocate_attribute("Type", _typeVal);
							lightXML->append_attribute(_typeAttrib);
						}

						//IsVisible
						{
							rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "IsVisible");
							const char* item_Val = xml->allocate_string(std::to_string(light->IsVisible()).c_str());
							item_XML->value(item_Val);

							lightXML->append_node(item_XML);
						}

						//Position
						{
							if (lightType == "LightPoint")
							{
								rapidxml::xml_node<>* position = xml->allocate_node(rapidxml::node_type::node_element, "Position");
								const char* pos_x_val = xml->allocate_string(std::to_string(((LightPoint*)light)->GetPosition().x).c_str());
								rapidxml::xml_attribute<char>* _x_Attrib = xml->allocate_attribute("X", pos_x_val);
								position->append_attribute(_x_Attrib);

								const char* pos_y_val = xml->allocate_string(std::to_string(((LightPoint*)light)->GetPosition().y).c_str());
								rapidxml::xml_attribute<char>* _y_Attrib = xml->allocate_attribute("Y", pos_y_val);
								position->append_attribute(_y_Attrib);

								const char* pos_z_val = xml->allocate_string(std::to_string(((LightPoint*)light)->GetPosition().z).c_str());
								rapidxml::xml_attribute<char>* _z_Attrib = xml->allocate_attribute("Z", pos_z_val);
								position->append_attribute(_z_Attrib);

								lightXML->append_node(position);
							}
							else if (lightType == "LightSpot")
							{
								rapidxml::xml_node<>* position = xml->allocate_node(rapidxml::node_type::node_element, "Position");
								const char* pos_x_val = xml->allocate_string(std::to_string(((LightSpot*)light)->GetPosition().x).c_str());
								rapidxml::xml_attribute<char>* _x_Attrib = xml->allocate_attribute("X", pos_x_val);
								position->append_attribute(_x_Attrib);

								const char* pos_y_val = xml->allocate_string(std::to_string(((LightSpot*)light)->GetPosition().y).c_str());
								rapidxml::xml_attribute<char>* _y_Attrib = xml->allocate_attribute("Y", pos_y_val);
								position->append_attribute(_y_Attrib);

								const char* pos_z_val = xml->allocate_string(std::to_string(((LightSpot*)light)->GetPosition().z).c_str());
								rapidxml::xml_attribute<char>* _z_Attrib = xml->allocate_attribute("Z", pos_z_val);
								position->append_attribute(_z_Attrib);

								lightXML->append_node(position);
							}
						}

						//Diffuse
						{
							Vector4D diffuse = light->GetDiffuse();
							rapidxml::xml_node<>* color_XML = xml->allocate_node(rapidxml::node_type::node_element, "Diffuse");
							const char* col_r_val = xml->allocate_string(std::to_string(diffuse.r).c_str());
							rapidxml::xml_attribute<char>* _r_Attrib = xml->allocate_attribute("R", col_r_val);
							color_XML->append_attribute(_r_Attrib);

							const char* col_g_val = xml->allocate_string(std::to_string(diffuse.g).c_str());
							rapidxml::xml_attribute<char>* _g_Attrib = xml->allocate_attribute("G", col_g_val);
							color_XML->append_attribute(_g_Attrib);

							const char* col_b_val = xml->allocate_string(std::to_string(diffuse.b).c_str());
							rapidxml::xml_attribute<char>* _b_Attrib = xml->allocate_attribute("B", col_b_val);
							color_XML->append_attribute(_b_Attrib);

							const char* col_a_val = xml->allocate_string(std::to_string(diffuse.a).c_str());
							rapidxml::xml_attribute<char>* _a_Attrib = xml->allocate_attribute("A", col_a_val);
							color_XML->append_attribute(_a_Attrib);

							lightXML->append_node(color_XML);
						}

						//Specular
						{
							Vector4D specular = light->GetSpecular();
							rapidxml::xml_node<>* color_XML = xml->allocate_node(rapidxml::node_type::node_element, "Specular");
							const char* col_r_val = xml->allocate_string(std::to_string(specular.r).c_str());
							rapidxml::xml_attribute<char>* _r_Attrib = xml->allocate_attribute("R", col_r_val);
							color_XML->append_attribute(_r_Attrib);

							const char* col_g_val = xml->allocate_string(std::to_string(specular.g).c_str());
							rapidxml::xml_attribute<char>* _g_Attrib = xml->allocate_attribute("G", col_g_val);
							color_XML->append_attribute(_g_Attrib);

							const char* col_b_val = xml->allocate_string(std::to_string(specular.b).c_str());
							rapidxml::xml_attribute<char>* _b_Attrib = xml->allocate_attribute("B", col_b_val);
							color_XML->append_attribute(_b_Attrib);

							const char* col_a_val = xml->allocate_string(std::to_string(specular.a).c_str());
							rapidxml::xml_attribute<char>* _a_Attrib = xml->allocate_attribute("A", col_a_val);
							color_XML->append_attribute(_a_Attrib);

							lightXML->append_node(color_XML);
						}

						//Direction
						{
							if (lightType == "LightDirectional")
							{
								Vector3D direction = ((LightDirectional*)light)->GetDirection();
								rapidxml::xml_node<>* xml_Item = xml->allocate_node(rapidxml::node_type::node_element, "Direction");
								const char* pos_x_val = xml->allocate_string(std::to_string(direction.x).c_str());
								rapidxml::xml_attribute<char>* _x_Attrib = xml->allocate_attribute("X", pos_x_val);
								xml_Item->append_attribute(_x_Attrib);

								const char* pos_y_val = xml->allocate_string(std::to_string(direction.y).c_str());
								rapidxml::xml_attribute<char>* _y_Attrib = xml->allocate_attribute("Y", pos_y_val);
								xml_Item->append_attribute(_y_Attrib);

								const char* pos_z_val = xml->allocate_string(std::to_string(direction.z).c_str());
								rapidxml::xml_attribute<char>* _z_Attrib = xml->allocate_attribute("Z", pos_z_val);
								xml_Item->append_attribute(_z_Attrib);

								lightXML->append_node(xml_Item);
							}
							else if (lightType == "LightSpot")
							{
								Vector3D direction = ((LightDirectional*)light)->GetDirection();
								rapidxml::xml_node<>* xml_Item = xml->allocate_node(rapidxml::node_type::node_element, "Direction");
								const char* pos_x_val = xml->allocate_string(std::to_string(direction.x).c_str());
								rapidxml::xml_attribute<char>* _x_Attrib = xml->allocate_attribute("X", pos_x_val);
								xml_Item->append_attribute(_x_Attrib);

								const char* pos_y_val = xml->allocate_string(std::to_string(direction.y).c_str());
								rapidxml::xml_attribute<char>* _y_Attrib = xml->allocate_attribute("Y", pos_y_val);
								xml_Item->append_attribute(_y_Attrib);

								const char* pos_z_val = xml->allocate_string(std::to_string(direction.z).c_str());
								rapidxml::xml_attribute<char>* _z_Attrib = xml->allocate_attribute("Z", pos_z_val);
								xml_Item->append_attribute(_z_Attrib);

								lightXML->append_node(xml_Item);
							}
						}

						//ConstAttenuation
						{
							if (lightType == "LightSpot")
							{
								rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "ConstAttenuation");
								const char* item_Val = xml->allocate_string(std::to_string(((LightSpot*)light)->GetConstantAttenuation()).c_str());
								item_XML->value(item_Val);

								lightXML->append_node(item_XML);
							}
							else if (lightType == "LightPoint")
							{
								rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "ConstAttenuation");
								const char* item_Val = xml->allocate_string(std::to_string(((LightPoint*)light)->GetConstantAttenuation()).c_str());
								item_XML->value(item_Val);

								lightXML->append_node(item_XML);
							}
						}

						//LinearAttenuation
						{
							if (lightType == "LightSpot")
							{
								rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "LinearAttenuation");
								const char* item_Val = xml->allocate_string(std::to_string(((LightSpot*)light)->GetLinearAttenuation()).c_str());
								item_XML->value(item_Val);

								lightXML->append_node(item_XML);
							}
							else if (lightType == "LightPoint")
							{
								rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "LinearAttenuation");
								const char* item_Val = xml->allocate_string(std::to_string(((LightPoint*)light)->GetLinearAttenuation()).c_str());
								item_XML->value(item_Val);

								lightXML->append_node(item_XML);
							}
						}

						//QuadraticAttenuation
						{
							if (lightType == "LightSpot")
							{
								rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "QuadraticAttenuation");
								const char* item_Val = xml->allocate_string(std::to_string(((LightSpot*)light)->GetQuadraticAttenuation()).c_str());
								item_XML->value(item_Val);

								lightXML->append_node(item_XML);
							}
							else if (lightType == "LightPoint")
							{
								rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "QuadraticAttenuation");
								const char* item_Val = xml->allocate_string(std::to_string(((LightPoint*)light)->GetQuadraticAttenuation()).c_str());
								item_XML->value(item_Val);

								lightXML->append_node(item_XML);
							}
						}

						//InnerAngle
						{
							if (lightType == "LightSpot")
							{
								rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "InnerAngle");
								const char* item_Val = xml->allocate_string(std::to_string(((LightSpot*)light)->GetInnerAngle()).c_str());
								item_XML->value(item_Val);

								lightXML->append_node(item_XML);
							}
						}

						//OuterAngle
						{
							if (lightType == "LightSpot")
							{
								rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "OuterAngle");
								const char* item_Val = xml->allocate_string(std::to_string(((LightSpot*)light)->GetOuterAngle()).c_str());
								item_XML->value(item_Val);

								lightXML->append_node(item_XML);
							}
						}

						//DistanceCutOff
						{
							if (lightType == "LightSpot")
							{
								rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "DistanceCutOff");
								const char* item_Val = xml->allocate_string(std::to_string(((LightSpot*)light)->GetCutoffDistance()).c_str());
								item_XML->value(item_Val);

								lightXML->append_node(item_XML);
							}
							else if (lightType == "LightPoint")
							{
								rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "DistanceCutOff");
								const char* item_Val = xml->allocate_string(std::to_string(((LightPoint*)light)->GetCutoffDistance()).c_str());
								item_XML->value(item_Val);

								lightXML->append_node(item_XML);
							}
						}

						lightsXML->append_node(lightXML);
					}
				}

				sceneXML->append_node(lightsXML);
			}
			
			//GameObjects
			{
				rapidxml::xml_node<>* gameObjectsXML = xml->allocate_node(rapidxml::node_type::node_element, "GameObjects");

				std::vector<Entity*> entities;
				if (scene->GetEntities(entities))
				{
					for (Entity*& entity : entities)
					{
						if (entity)
						{
							std::string goType = entity->GetType();

							bool isGrouped = false; // IsObjectGrouped(*level, *go);

							rapidxml::xml_node<char>* goXML = xml->allocate_node(rapidxml::node_type::node_element, "GameObject");

							//Name
							{
								const char* _nameVal = xml->allocate_string(entity->GetName().c_str());
								rapidxml::xml_attribute<char>* _nameAttrib = xml->allocate_attribute("Name", _nameVal);
								goXML->append_attribute(_nameAttrib);
							}

							//Type
							{
								const char* _typeVal = xml->allocate_string(goType.c_str());
								rapidxml::xml_attribute<char>* _typeAttrib = xml->allocate_attribute("Type", _typeVal);
								goXML->append_attribute(_typeAttrib);
							}

							//Mesh
							{
							/*	std::string pathOfMesh = go->meshes[0]->GetPath().substr(go->meshes[0]->GetPath().find_last_of("/\\") + 1);
								const char* _meshVal = xml->allocate_string(pathOfMesh.c_str());
								rapidxml::xml_attribute<char>* _meshAttrib = xml->allocate_attribute("Mesh", _meshVal);
								goXML->append_attribute(_meshAttrib);*/
							}

							//Position
							{
								rapidxml::xml_node<>* position = xml->allocate_node(rapidxml::node_type::node_element, "Position");
								const char* pos_x_val = xml->allocate_string(isGrouped ? "0" : std::to_string(entity->GetComponent<IRigidbody*>()->GetPosition().x).c_str());
								rapidxml::xml_attribute<char>* _x_Attrib = xml->allocate_attribute("X", pos_x_val);
								position->append_attribute(_x_Attrib);

								const char* pos_y_val = xml->allocate_string(isGrouped ? "0" : std::to_string(entity->GetComponent<IRigidbody*>()->GetPosition().y).c_str());
								rapidxml::xml_attribute<char>* _y_Attrib = xml->allocate_attribute("Y", pos_y_val);
								position->append_attribute(_y_Attrib);

								const char* pos_z_val = xml->allocate_string(isGrouped ? "0" : std::to_string(entity->GetComponent<IRigidbody*>()->GetPosition().z).c_str());
								rapidxml::xml_attribute<char>* _z_Attrib = xml->allocate_attribute("Z", pos_z_val);
								position->append_attribute(_z_Attrib);

								goXML->append_node(position);
							}

							//Rotation
							{
								Vector3D eulerRot = entity->GetComponent<IRigidbody*>()->GetRotation_Euler();

								rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "Rotation");
								const char* item_x_val = xml->allocate_string(isGrouped ? "0" : std::to_string(eulerRot.x).c_str());
								rapidxml::xml_attribute<char>* _x_Attrib = xml->allocate_attribute("X", item_x_val);
								item_XML->append_attribute(_x_Attrib);

								const char* item_y_val = xml->allocate_string(isGrouped ? "0" : std::to_string(eulerRot.y).c_str());
								rapidxml::xml_attribute<char>* _y_Attrib = xml->allocate_attribute("Y", item_y_val);
								item_XML->append_attribute(_y_Attrib);

								const char* item_z_val = xml->allocate_string(isGrouped ? "0" : std::to_string(eulerRot.z).c_str());
								rapidxml::xml_attribute<char>* _z_Attrib = xml->allocate_attribute("Z", item_z_val);
								item_XML->append_attribute(_z_Attrib);

								goXML->append_node(item_XML);
							}

							//DiffuseColor
							{
								/*rapidxml::xml_node<>* color_XML = xml->allocate_node(rapidxml::node_type::node_element, "DiffuseColor");
								const char* col_r_val = xml->allocate_string(std::to_string(((GameObject*)go)->diffuseColor.r).c_str());
								rapidxml::xml_attribute<char>* _r_Attrib = xml->allocate_attribute("R", col_r_val);
								color_XML->append_attribute(_r_Attrib);

								const char* col_g_val = xml->allocate_string(std::to_string(((GameObject*)go)->diffuseColor.g).c_str());
								rapidxml::xml_attribute<char>* _g_Attrib = xml->allocate_attribute("G", col_g_val);
								color_XML->append_attribute(_g_Attrib);

								const char* col_b_val = xml->allocate_string(std::to_string(((GameObject*)go)->diffuseColor.b).c_str());
								rapidxml::xml_attribute<char>* _b_Attrib = xml->allocate_attribute("B", col_b_val);
								color_XML->append_attribute(_b_Attrib);

								const char* col_a_val = xml->allocate_string(std::to_string(((GameObject*)go)->diffuseColor.a).c_str());
								rapidxml::xml_attribute<char>* _a_Attrib = xml->allocate_attribute("A", col_a_val);
								color_XML->append_attribute(_a_Attrib);

								goXML->append_node(color_XML);*/
							}

							//SpecularColor
							{
							/*	rapidxml::xml_node<>* color_XML = xml->allocate_node(rapidxml::node_type::node_element, "SpecularColor");
								const char* col_r_val = xml->allocate_string(std::to_string(((GameObject*)go)->specularColor.r).c_str());
								rapidxml::xml_attribute<char>* _r_Attrib = xml->allocate_attribute("R", col_r_val);
								color_XML->append_attribute(_r_Attrib);

								const char* col_g_val = xml->allocate_string(std::to_string(((GameObject*)go)->specularColor.g).c_str());
								rapidxml::xml_attribute<char>* _g_Attrib = xml->allocate_attribute("G", col_g_val);
								color_XML->append_attribute(_g_Attrib);

								const char* col_b_val = xml->allocate_string(std::to_string(((GameObject*)go)->specularColor.b).c_str());
								rapidxml::xml_attribute<char>* _b_Attrib = xml->allocate_attribute("B", col_b_val);
								color_XML->append_attribute(_b_Attrib);

								const char* col_a_val = xml->allocate_string(std::to_string(((GameObject*)go)->specularColor.w).c_str());
								rapidxml::xml_attribute<char>* _a_Attrib = xml->allocate_attribute("W", col_a_val);
								color_XML->append_attribute(_a_Attrib);

								goXML->append_node(color_XML);*/
							}

							//Speed
							{
								/*rapidxml::xml_node<>* speed_XML = xml->allocate_node(rapidxml::node_type::node_element, "Speed");
								const char* spd_x_val = xml->allocate_string(isGrouped ? "0" : std::to_string(go->speed.x).c_str());
								rapidxml::xml_attribute<char>* _x_Attrib = xml->allocate_attribute("X", spd_x_val);
								speed_XML->append_attribute(_x_Attrib);

								const char* spd_y_val = xml->allocate_string(isGrouped ? "0" : std::to_string(go->speed.y).c_str());
								rapidxml::xml_attribute<char>* _y_Attrib = xml->allocate_attribute("Y", spd_y_val);
								speed_XML->append_attribute(_y_Attrib);

								const char* spd_z_val = xml->allocate_string(isGrouped ? "0" : std::to_string(go->speed.z).c_str());
								rapidxml::xml_attribute<char>* _z_Attrib = xml->allocate_attribute("Z", spd_z_val);
								speed_XML->append_attribute(_z_Attrib);

								goXML->append_node(speed_XML);*/
							}

							//Acceleration
							{
							/*	rapidxml::xml_node<>* acc_XML = xml->allocate_node(rapidxml::node_type::node_element, "Acceleration");
								const char* acc_x_val = xml->allocate_string(isGrouped ? "0" : std::to_string(go->acceleration.x).c_str());
								rapidxml::xml_attribute<char>* _x_Attrib = xml->allocate_attribute("X", acc_x_val);
								acc_XML->append_attribute(_x_Attrib);

								const char* acc_y_val = xml->allocate_string(isGrouped ? "0" : std::to_string(go->acceleration.y).c_str());
								rapidxml::xml_attribute<char>* _y_Attrib = xml->allocate_attribute("Y", acc_y_val);
								acc_XML->append_attribute(_y_Attrib);

								const char* acc_z_val = xml->allocate_string(isGrouped ? "0" : std::to_string(go->acceleration.z).c_str());
								rapidxml::xml_attribute<char>* _z_Attrib = xml->allocate_attribute("Z", acc_z_val);
								acc_XML->append_attribute(_z_Attrib);

								goXML->append_node(acc_XML);*/
							}

							//AllowCollisionDetection
							{
								/*rapidxml::xml_node<>* bool_XML = xml->allocate_node(rapidxml::node_type::node_element, "AllowCollisionDetection");
								const char* boolVal = xml->allocate_string(go->allowCollisionDetection ? "1" : "0");
								bool_XML->value(boolVal);

								goXML->append_node(bool_XML);*/
							}

							IRender* vr = entity->GetComponent<IRender*>();
							if (vr)
							{
								//IsVisible
								{
									rapidxml::xml_node<>* isvisible_XML = xml->allocate_node(rapidxml::node_type::node_element, "IsVisible");
									const char* boolVal = xml->allocate_string(vr->IsVisible() ? "1" : "0");
									isvisible_XML->value(boolVal);

									goXML->append_node(isvisible_XML);
								}

								//VisibilityMode
								{
									rapidxml::xml_node<>* visibilityMode_XML = xml->allocate_node(rapidxml::node_type::node_element, "VisibilityMode");

									if (vr->GetRenderMode() == GL_FILL)
									{
										visibilityMode_XML->value("Fill");
									}
									else if (vr->GetRenderMode() == GL_LINE)
									{
										visibilityMode_XML->value("Wireframe");
									}
									else
									{
										visibilityMode_XML->value("Fill");
									}

									goXML->append_node(visibilityMode_XML);
								}

								//ShaderFeatures
								{
									IRender* vr = entity->GetComponent<IRender*>();

									if (vr)
									{
										rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "ShaderFeatures");
										goXML->append_node(item_XML);

										std::vector<ShaderFeature*> shaderFeatures;
										vr->GetShaderFeatures(shaderFeatures);

										for (unsigned int j = 0; j < shaderFeatures.size(); j++)
										{
											ShaderFeature* sf = shaderFeatures[j];

											rapidxml::xml_node<>* sfXML = xml->allocate_node(rapidxml::node_type::node_element, "ShaderFeature");

											//Name
											{
												const char* _nameVal = xml->allocate_string(sf->GetName().c_str());
												rapidxml::xml_attribute<char>* _nameAttrib = xml->allocate_attribute("Name", _nameVal);
												sfXML->append_attribute(_nameAttrib);
											}

											//Parameters
											{
												rapidxml::xml_node<>* parameters = xml->allocate_node(rapidxml::node_type::node_element, "Parameters");

												std::vector<Parameter> parametersVec;
												if (sf->parameters.GetParameters(parametersVec))
												{
													for (Parameter par : parametersVec)
													{
														//Name
														const char* _parName = xml->allocate_string(par.name.c_str());
														rapidxml::xml_node<>* parameter = xml->allocate_node(rapidxml::node_type::node_element, _parName);

														//Type
														{
															const char* _parType = xml->allocate_string(par.type.c_str());
															rapidxml::xml_attribute<char>* _typeAttrib = xml->allocate_attribute("Type", _parType);
															parameter->append_attribute(_typeAttrib);
														}

														//Value
														{
															const char* _parValue = xml->allocate_string(par.value.c_str());
															parameter->value(_parValue);
														}

														parameters->append_node(parameter);
													}

													sfXML->append_node(parameters);
												}
											}

											item_XML->append_node(sfXML);
										}
									}
								}
							}

							//Scale
							{
								rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "Scale");
								const char* item_x_val = xml->allocate_string(isGrouped ? "0" : std::to_string(entity->GetComponent<IRigidbody*>()->GetScale().x).c_str());
								rapidxml::xml_attribute<char>* _x_Attrib = xml->allocate_attribute("X", item_x_val);
								item_XML->append_attribute(_x_Attrib);

								const char* item_y_val = xml->allocate_string(isGrouped ? "0" : std::to_string(entity->GetComponent<IRigidbody*>()->GetScale().y).c_str());
								rapidxml::xml_attribute<char>* _y_Attrib = xml->allocate_attribute("Y", item_y_val);
								item_XML->append_attribute(_y_Attrib);

								const char* item_z_val = xml->allocate_string(isGrouped ? "0" : std::to_string(entity->GetComponent<IRigidbody*>()->GetScale().z).c_str());
								rapidxml::xml_attribute<char>* _z_Attrib = xml->allocate_attribute("Z", item_z_val);
								item_XML->append_attribute(_z_Attrib);

								goXML->append_node(item_XML);
							}

							//InverseMass
							{
								/*	rapidxml::xml_node<>* inverseMass_XML = xml->allocate_node(rapidxml::node_type::node_element, "InverseMass");
									const char* inverseMass_val = xml->allocate_string(std::to_string(go->inverseMass).c_str());
									inverseMass_XML->value(inverseMass_val);

									goXML->append_node(inverseMass_XML);*/
							}

							//CollisionData
							{
								/*rapidxml::xml_node<>* item_XML = xml->allocate_node(rapidxml::node_type::node_element, "Collider");

								const char* shape_val = xml->allocate_string(go->collider->GetType().c_str());
								rapidxml::xml_attribute<char>* _shape_Attrib = xml->allocate_attribute("Shape", shape_val);
								item_XML->append_attribute(_shape_Attrib);

								goXML->append_node(item_XML);*/
							}

							gameObjectsXML->append_node(goXML);
						}
					}

				}
				
				sceneXML->append_node(gameObjectsXML);
			}

			root->append_node(sceneXML);
			xml->append_node(root);
		}

		//Debug
		{
		/*	std::string xmlStr;
			rapidxml::print(std::back_inserter(xmlStr), *xml);*/
		}		
	}
}

std::string StateSaver::GetTimeStamp(bool time, bool date)
{
	std::time_t t = std::time(0);   // get time now
	std::tm now;
	localtime_s(&now, &t);

	char day[3];
	sprintf_s(day, 3, "%02d", now.tm_mday);
	char month[3];
	sprintf_s(month, 3, "%02d", now.tm_mon + 1);
	char year[5];
	sprintf_s(year, 5, "%04d", now.tm_year + 1900);

	char hour[3];
	sprintf_s(hour, 3, "%02d", now.tm_hour);
	char min[3];
	sprintf_s(min, 3, "%02d", now.tm_min);
	char sec[3];
	sprintf_s(sec, 3, "%02d", now.tm_sec);

	std::string stamp;
	if (time && date)
	{
		stamp = std::string(hour) + std::string(min) + std::string(sec) + "_" + std::string(day) + std::string(month) + std::string(year);
	}
	else
	{
		if (time)
		{
			stamp = std::string(hour) + std::string(min) + std::string(sec);
		}
		else if(date)
		{
			stamp = std::string(day) + std::string(month) + std::string(year);
		}
		else
		{
			//No stamp
		}
	}
	
	return stamp;
}

std::string StateSaver::SaveXML(rapidxml::xml_document<>* xml, std::string directory, std::string filename)
{
	struct stat info;
	//Checks if the directory exists and creates it if it doesn't
	if (stat(directory.c_str(), &info) != 0)
	{ 
		int rc = _mkdir(directory.c_str());
		if (rc != 0)
		{

		}
	}
	else
	{
		if (info.st_mode & S_IFDIR)
		{
			
		}
		else 
		{
			char err[255];
			sprintf_s(err, 255, "%s is not a directory.", directory.c_str());
			throw std::exception(err);
		}
	}

	std::string filePath = directory + "\\" + filename;
	std::ofstream file_stored(filePath);
	file_stored << *xml;
	file_stored.close();

	return filePath;
}

std::string StateSaver::SaveState(Level* level)
{
	std::string filePath;

	rapidxml::xml_document<>* xml = new rapidxml::xml_document<>();
	if (xml)
	{
		WriteXMLHeader("1.0", "utf-8", xml);
		Serialize(level, xml);
				
		std::string filename = level->GetName() + "_SS_" + GetTimeStamp() + ".xml";
		std::string folder = EngineFolders::workingDirectory + "\\" + FOLDER_SAVE;

		filePath = SaveXML(xml, folder, filename);

		delete (xml);
	}	

	return filePath;
}
