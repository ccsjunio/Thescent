#include <GameSystemVideo.h>
#include <TextureHeightMap.h>
#include <LightSpot.h>
#include <LightPoint.h>
#include <LightDirectional.h>
#include <IRigidbody.h>
#include <ISkinnedMesh.h>
#include <StencilBuffer.h>
#include <IRender.h>
#include <IMeshComponent.h>
#include <BellEngineID.h>
#include <IMesh.h>
#include <ModelDef.h>
#include <IParticleMeshComponent.h>
#include <IParticleEmitter.h>
#include <Shaders.h>

#include <OpenGLError.h>
#include <StringHelpers.h>
/*
	//FOR DEBUG
	#include <OpenGLError.h>
	OpenGLError glerror; bool glerr; GLenum errCode; std::string errorStr, errorDet;
	glerr = glerror.bWasThereAnOpenGLError(errCode, errorStr, errorDet);
*/

constexpr auto SHADER_LOC_VS_COLOR =								1u;
constexpr auto SHADER_LOC_VS_POSITION =								2u;
constexpr auto SHADER_LOC_VS_NORMAL =								3u;
constexpr auto SHADER_LOC_VS_TEXTURE =								4u;
constexpr auto SHADER_LOC_VS_TANGENT =								5u;
constexpr auto SHADER_LOC_VS_BINORMAL =								6u;
constexpr auto SHADER_LOC_VS_BONEID =								7u;
constexpr auto SHADER_LOC_VS_BONEWEIGHT =							8u;

//SHADER INDICES
constexpr auto SHADER_INDEX_VERTEX_BASIC =							0u;
constexpr auto SHADER_INDEX_VERTEX_INSTANCED =						1u;
constexpr auto SHADER_INDEX_VERTEX_SKINNEDMESH =					2u;
constexpr auto SHADER_INDEX_FRAGMENT_COLORIZE =						3u;
constexpr auto SHADER_INDEX_FRAGMENT_TEXTURIZE =					4u;
constexpr auto SHADER_INDEX_FRAGMENT_TEXTURIZESKYBOX =				5u;
constexpr auto SHADER_INDEX_FRAGMENT_INTERPOLATION_COLORIZE	=		6u;
constexpr auto SHADER_INDEX_FRAGMENT_IMPOSTER =						7u;
constexpr auto SHADER_INDEX_FRAGMENT_IMPOSTER_V2 =					8u;

GameSystemVideo* GameSystemVideo::systemVideo = nullptr;

void GameSystemVideo::WriteShaderFeatures(std::vector<ShaderFeature*>& shaderFeatures, IMesh*& mesh, IMeshComponent*& meshComponent)
{
	if (!shaderFeatures.empty())
	{
		IShaderProgram* program = this->shaderContainer->FindShaderProgramByAllFeatures(shaderFeatures);
		if (!program) return;

		std::string featureName;

		/*FRAGMENT SHADER*/
		size_t fragmentShaderCount = 0;
		for (size_t i = 0; i < shaderFeatures.size(); i++)
		{
			ShaderFeature* shaderFeature = shaderFeatures[i];
			if (shaderFeature->GetShaderType() != "fragment")
			{
				continue;
			}

			featureName = shaderFeature->GetName();

			if (featureName == "DISCARD_BY_HEIGHT") //ShaderID = 1
			{
				int shaderID = 1;

				glUseProgram(program->GetID());

				char buf[100];
				sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);

				GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
				glUniform1i(shaderFeatureArray, shaderID);

				Parameter p;
				bool rc = shaderFeature->parameters.FindParameterByName("HEIGHT_THRESHOLD", p);
				if (rc)
				{
					float HEIGHT_THRESHOLD_VALUE = (float)atof(p.value.c_str());

					GLint discardByHeightThreshold = glGetUniformLocation(program->GetID(), "discardByHeightThreshold");
					glUniform1f(discardByHeightThreshold, HEIGHT_THRESHOLD_VALUE);
				}
			}
			else if (featureName == "TEXTURIZE") //ShaderID = 6
			{
				int shaderID = 6;

				IGLShader* sd = glShaders[SHADER_INDEX_FRAGMENT_TEXTURIZE];
				if (sd)
				{
					char buf[100];
					sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);
					GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
					glUniform1i(shaderFeatureArray, shaderID);
					//---------------------------------------------------------------------------------------

					TexturizeFragmentShader* shader = (TexturizeFragmentShader*)sd;
					shader->SetProgramID(program->GetID());

					std::vector<ITexture*> textures;
					meshComponent->GetMeshTextures(mesh, textures);
					shader->SetTextures(textures);
										
					/*Check for light components. These are lights that are tied to the entitites and will be transformed relative to it.*/
					std::vector<ILight*> all_lights, scene_lights;
					auto sceneLights_ptr = scene->GetLights();
					if (sceneLights_ptr)
					{
						scene_lights = *sceneLights_ptr;
						for (auto& l : scene_lights) all_lights.push_back(l);
					}
					shader->SetLights(all_lights);
					
					Parameter p;

					Vector4D diffuse = Vector4D(1.0, 0.0f, 0.0f, 1.0f);
					if (shaderFeature->parameters.FindParameterByName("COLOR_DIFFUSE", p)) diffuse = StringHelpers::StrToVec4(p.value);
					shader->SetDiffuse(diffuse);

					Vector4D specular = Vector4D(1.0, 1.0f, 1.0f, 1000.0f);
					if (shaderFeature->parameters.FindParameterByName("COLOR_SPECULAR", p)) specular = StringHelpers::StrToVec4(p.value);
					shader->SetSpecular(specular);

					int TRANSPARENCY = 0;
					if (shaderFeature->parameters.FindParameterByName("TRANSPARENCY", p))TRANSPARENCY = strtoul(p.value.c_str(), NULL, 0);
					shader->SetTransparency(TRANSPARENCY);

					unsigned long offsetType = 0ull; /*0 - Fixed, 1 - Constant, 2 - Over time*/
					if (shaderFeature->parameters.FindParameterByName("OFFSET_TYPE", p)) offsetType = strtoul(p.value.c_str(), NULL, 0);

					Vector2D uvoffset = Vector2D(0.0f, 0.0f);
					if (shaderFeature->parameters.FindParameterByName("UVOFFSET", p)) uvoffset = StringHelpers::StrToVec2(p.value);

					switch (offsetType)
					{
					/*Offset is constant*/
					case 1:
					{
						/*Check for an internal parameter in the shaderfeature. This is where the last uvoffsets will be stored.*/
						if (!shaderFeature->parameters.HasParameter("LASTUVOFFSET"))
						{
							/*Updates offfset*/
							uvoffset += uvoffset;
							/*Does not exist. Create variable and store in the container.*/
							shaderFeature->parameters.Add("Vector2D", "LASTUVOFFSET", StringHelpers::Vec2ToStr(uvoffset), true);
						}
						else
						{
							/*Exists, replaces the old value with the current one.*/
							if (shaderFeature->parameters.FindParameterByName("LASTUVOFFSET", p))
							{
								Vector2D lastOffset = StringHelpers::StrToVec2(p.value);
								
								/*Updates offfset*/
								uvoffset = lastOffset + uvoffset * (float)frameTime.GetDeltaTime();

								shaderFeature->parameters.SetValue("LASTUVOFFSET", StringHelpers::Vec2ToStr(uvoffset));
							}
							else { /*Not possible!*/ }
						}
					}
					break;
					/*Offset changes over time*/
					case 2:
					{
						/*Check for an internal parameter in the shaderfeature. This is where the last uvoffsets will be stored.*/
						if (!shaderFeature->parameters.HasParameter("LASTUVOFFSET"))
						{
							/*Updates offfset*/
							uvoffset += uvoffset * (float)frameTime.GetDeltaTime();
							/*Does not exist. Create variable and store in the container.*/
							shaderFeature->parameters.Add("Vector2D", "LASTUVOFFSET", StringHelpers::Vec2ToStr(uvoffset), true);
						}
						else
						{
							/*Exists, replaces the old value with the current one.*/
							if (shaderFeature->parameters.FindParameterByName("LASTUVOFFSET", p))
							{
								Vector2D lastOffset = StringHelpers::StrToVec2(p.value);
								uvoffset = lastOffset;

								/*Updates offfset*/
								uvoffset += uvoffset * (float)frameTime.GetDeltaTime();

								shaderFeature->parameters.SetValue("LASTUVOFFSET", StringHelpers::Vec2ToStr(uvoffset));
							}
							else { /*Not possible!*/ }
						}
					}
					break;
					case 0:
					default:
					{

					}
					break;
					}

					/*Sets the offset in the shader.*/
					shader->SetUVOffset(uvoffset);

					Vector2D tiling = Vector2D(1.0f, 1.0f);
					if (shaderFeature->parameters.FindParameterByName("TILING", p))
					{
						tiling = StringHelpers::StrToVec2(p.value);
						if (tiling.x == 0.0f) tiling.x = 1.0f;
						if (tiling.y == 0.0f) tiling.y = 1.0f;
					}
					shader->SetTiling(tiling);
				}
			}
			else if (featureName == "TEXTURIZE_SKYBOX") //ShaderID = 7
			{
				int shaderID = 7;
								
				glUseProgram(program->GetID());

				char buf[100];
				sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);

				GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
				glUniform1i(shaderFeatureArray, shaderID);
			}
			else if (featureName == "COLORIZE") //ShaderID = 9
			{
				int shaderID = 9;

				IGLShader* sd = glShaders[SHADER_INDEX_FRAGMENT_COLORIZE];
				if (sd)
				{
					char buf[100];
					sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);
					GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
					glUniform1i(shaderFeatureArray, shaderID);
					//---------------------------------------------------------------------------------------

					ColorizeFragmentShader* shader = (ColorizeFragmentShader*)sd;
					shader->SetProgramID(program->GetID());

					/*Check for light components. These are lights that are tied to the entitites and will be transformed relative to it.*/
					std::vector<ILight*> all_lights, scene_lights;
					auto sceneLights_ptr = scene->GetLights();
					if (sceneLights_ptr)
					{
						scene_lights = *sceneLights_ptr;
						for (auto& l : scene_lights) all_lights.push_back(l);
					}
					shader->SetLights(all_lights);

					Parameter p;
					Vector4D diffuse = Vector4D(1.0, 0.0f, 0.0f, 1.0f);
					if (shaderFeature->parameters.FindParameterByName("COLOR_DIFFUSE", p)) diffuse = StringHelpers::StrToVec4(p.value);
					shader->SetDiffuse(diffuse);

					Vector4D specular = Vector4D(1.0, 1.0f, 1.0f, 1000.0f);
					if (shaderFeature->parameters.FindParameterByName("COLOR_SPECULAR", p)) specular = StringHelpers::StrToVec4(p.value);
					shader->SetSpecular(specular);
									
					unsigned int isLit = 1u;
					if (shaderFeature->parameters.FindParameterByName("IsLit", p)) isLit = strtoul(p.value.c_str(), NULL, 0);
					shader->SetIsLit(isLit != 0u);
				}
			}
			else if (featureName == "IMPOSTER") //ShaderID = 10
			{
				int shaderID = 10;
								
				IGLShader* sd = glShaders[SHADER_INDEX_FRAGMENT_IMPOSTER_V2];
				if (sd)
				{
					char buf[100];
					sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);
					GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
					glUniform1i(shaderFeatureArray, shaderID);
					//---------------------------------------------------------------------------------------

					ImposterFragmentShaderV2* shader = (ImposterFragmentShaderV2*)sd;
					shader->SetProgramID(program->GetID());
						
					Parameter p;

					Vector4D diffuse = Vector4D(1.0, 0.0f, 0.0f, 1.0f);
					if (shaderFeature->parameters.FindParameterByName("COLOR_DIFFUSE", p)) diffuse = StringHelpers::StrToVec4(p.value);
					shader->SetDiffuse(diffuse);

					Vector4D specular = Vector4D(1.0, 1.0f, 1.0f, 1000.0f);
					if (shaderFeature->parameters.FindParameterByName("COLOR_SPECULAR", p)) specular = StringHelpers::StrToVec4(p.value);
					shader->SetSpecular(specular);

					std::vector<ITexture*> textures;
					meshComponent->GetMeshTextures(mesh, textures);
					shader->SetTextures(textures);
								
					int TRANSPARENCY = 0;
					if (shaderFeature->parameters.FindParameterByName("TRANSPARENCY", p))TRANSPARENCY = strtoul(p.value.c_str(), NULL, 0);
					shader->SetTransparency(TRANSPARENCY);

					Vector2D uvoffset = Vector2D(0.0f, 0.0f);
					if (shaderFeature->parameters.FindParameterByName("UVOFFSET", p)) uvoffset = StringHelpers::StrToVec2(p.value);
					shader->SetUVOffset(uvoffset);

					Vector2D tiling = Vector2D(1.0f, 1.0f);
					if (shaderFeature->parameters.FindParameterByName("TILING", p))
					{
						tiling = StringHelpers::StrToVec2(p.value);
						if (tiling.x == 0.0f) tiling.x = 1.0f;
						if (tiling.y == 0.0f) tiling.y = 1.0f;
					}
					shader->SetTiling(tiling);
				}
			}
			else if (featureName == "REFLECTREFRACT") //ShaderID = 11
			{
				int shaderID = 11;

				glUseProgram(program->GetID());

				char buf[100];
				sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);

				GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
				glUniform1i(shaderFeatureArray, shaderID);
			}
			else if (featureName == "TEXTURE_OFFSET") //ShaderID = 12
			{
				int shaderID = 12;

				glUseProgram(program->GetID());

				char buf[100];
				sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);

				GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
				glUniform1i(shaderFeatureArray, shaderID);

				Vector2D offsetOverTime = Vector2D();
				Parameter p;
				bool rc = false;

				rc = shaderFeature->parameters.FindParameterByName("OFFSET_X_OVER_TIME", p);
				if (rc)
				{
					offsetOverTime.x = (float)atof(p.value.c_str());
				}
				rc = shaderFeature->parameters.FindParameterByName("OFFSET_Y_OVER_TIME", p);
				if (rc)
				{
					offsetOverTime.y = (float)atof(p.value.c_str());
				}
				rc = shaderFeature->parameters.FindParameterByName("OFFSET_X_OVER_TIME_VAR", p);
				if (rc)
				{
					float lastOffsetOverTimeX = (float)atof(p.value.c_str());
					lastOffsetOverTimeX += offsetOverTime.x * frameTime.GetDeltaTime();
					offsetOverTime.x = lastOffsetOverTimeX;
					//Update value
					rc = shaderFeature->parameters.SetValue(p.name, std::to_string(lastOffsetOverTimeX));
				}
				rc = shaderFeature->parameters.FindParameterByName("OFFSET_Y_OVER_TIME_VAR", p);
				if (rc)
				{
					float lastOffsetOverTimeY = (float)atof(p.value.c_str());
					lastOffsetOverTimeY += offsetOverTime.y * frameTime.GetDeltaTime();
					offsetOverTime.y = lastOffsetOverTimeY;
					//Update value
					rc = shaderFeature->parameters.SetValue(p.name, std::to_string(lastOffsetOverTimeY));
				}

				GLint textureOffsetVec = glGetUniformLocation(program->GetID(), "textureOffsetVec");
				glUniform2f(textureOffsetVec, offsetOverTime.x, offsetOverTime.y);

				rc = shaderFeature->parameters.FindParameterByName("TEXTURE_ID", p);
				if (rc)
				{
					int TEXTURE_ID = atoi(p.value.c_str());

					GLint textureID = glGetUniformLocation(program->GetID(), "textureID");
					glUniform1i(textureID, TEXTURE_ID);
				}
			}
			else if (featureName == "TEXTURE_TWO_BYRATIO") //ShaderID = 13
			{
				int shaderID = 13;

				glUseProgram(program->GetID());

				char buf[100];
				sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);

				GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
				glUniform1i(shaderFeatureArray, shaderID);

				Parameter p;
				bool rc = shaderFeature->parameters.FindParameterByName("TEXTURE_0_ID", p);
				if (rc)
				{
					int TEXTURE_0_ID = atoi(p.value.c_str());

					GLint textureTwoByRatioIDs0 = glGetUniformLocation(program->GetID(), "textureTwoByRatioIDs[0]");
					glUniform1i(textureTwoByRatioIDs0, TEXTURE_0_ID);
				}

				rc = shaderFeature->parameters.FindParameterByName("TEXTURE_1_ID", p);
				if (rc)
				{
					int TEXTURE_1_ID = atoi(p.value.c_str());

					GLint textureTwoByRatioIDs1 = glGetUniformLocation(program->GetID(), "textureTwoByRatioIDs[1]");
					glUniform1i(textureTwoByRatioIDs1, TEXTURE_1_ID);
				}


				rc = shaderFeature->parameters.FindParameterByName("RATIO_0", p);
				if (rc)
				{
					float RATIO0 = (float)atof(p.value.c_str());

					GLint ratio0 = glGetUniformLocation(program->GetID(), "textureTwoByRatios[0]");
					glUniform1f(ratio0, RATIO0);
				}

				rc = shaderFeature->parameters.FindParameterByName("RATIO_1", p);
				if (rc)
				{
					float RATIO1 = (float)atof(p.value.c_str());

					GLint ratio1 = glGetUniformLocation(program->GetID(), "textureTwoByRatios[1]");
					glUniform1f(ratio1, RATIO1);
				}
			}
			else if (featureName == "USE_HEIGHTMAP") //ShaderID = 14
			{
				int shaderID = 14;

				glUseProgram(program->GetID());

				char buf[100];
				sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);

				GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
				glUniform1i(shaderFeatureArray, shaderID);

				Vector2D offsetOverTime = Vector2D();
				Parameter p;
				bool rc = shaderFeature->parameters.FindParameterByName("OFFSET_X_OVER_TIME", p);
				if (rc)
				{
					offsetOverTime.x = (float)atof(p.value.c_str());
				}
				rc = shaderFeature->parameters.FindParameterByName("OFFSET_Y_OVER_TIME", p);
				if (rc)
				{
					offsetOverTime.y = (float)atof(p.value.c_str());
				}
				rc = shaderFeature->parameters.FindParameterByName("OFFSET_X_OVER_TIME_VAR", p);
				if (rc)
				{
					float lastOffsetOverTimeX = (float)atof(p.value.c_str());
					lastOffsetOverTimeX += offsetOverTime.x * frameTime.GetDeltaTime();
					offsetOverTime.x = lastOffsetOverTimeX;

					shaderFeature->parameters.SetValue(p.name, std::to_string(lastOffsetOverTimeX));
				}
				rc = shaderFeature->parameters.FindParameterByName("OFFSET_Y_OVER_TIME_VAR", p);
				if (rc)
				{
					float lastOffsetOverTimeY = (float)atof(p.value.c_str());
					lastOffsetOverTimeY += offsetOverTime.y * frameTime.GetDeltaTime();
					offsetOverTime.y = lastOffsetOverTimeY;

					shaderFeature->parameters.SetValue(p.name, std::to_string(lastOffsetOverTimeY));
				}

				GLint textureOffsetVec = glGetUniformLocation(program->GetID(), "textureOffsetVec");
				glUniform2f(textureOffsetVec, offsetOverTime.x, offsetOverTime.y);

				rc = shaderFeature->parameters.FindParameterByName("TEXTURE_ID", p);
				if (rc)
				{
					int TEXTURE_ID = atoi(p.value.c_str());

					GLint textureID = glGetUniformLocation(program->GetID(), "textureID");
					glUniform1i(textureID, TEXTURE_ID);
				}
			}
			else
			{
				continue;
			}

			fragmentShaderCount++;
		}
				
		GLint shaderFeatureArrayCount = glGetUniformLocation(program->GetID(), "fragmentShaderInfo.shaderFeatureArrayCount");
		glUniform1i(shaderFeatureArrayCount, (GLint)fragmentShaderCount);

		/*VERTEX SHADER*/
		size_t vertexShaderCount = 0;
		for (size_t i = 0; i < shaderFeatures.size(); i++)
		{
			ShaderFeature* shaderFeature = shaderFeatures[i];
			if (shaderFeature->GetShaderType() != "vertex")
			{
				continue;
			}

			featureName = shaderFeature->GetName();
			if (featureName == "BASIC_VERTEX_DRAW")
			{
				int shaderID = 1;
						
				ResolveCamera(program->GetID());

				glUseProgram(program->GetID());

				char buf[100];
				sprintf_s(buf, 100, "vertexShaderInfo.shaderFeatureArray[%llu]", vertexShaderCount);

				Parameter p;
				int ENABLE_OFFSET_ALONG_NORMAL = 0;
				if (shaderFeature->parameters.FindParameterByName("ENABLE_OFFSET_ALONG_NORMAL", p))
				{
					ENABLE_OFFSET_ALONG_NORMAL = strtoul(p.value.c_str(), NULL, 0);
				}
				GLint ENABLE_OFFSET_ALONG_NORMAL_UL = glGetUniformLocation(program->GetID(), "shaderBasicVertexInfo.offsetAlongNormal");
				glUniform1i(ENABLE_OFFSET_ALONG_NORMAL_UL, ENABLE_OFFSET_ALONG_NORMAL);

				float VERTEX_OFFSET = 0.0f;
				if (shaderFeature->parameters.FindParameterByName("VERTEX_OFFSET", p))
				{
					VERTEX_OFFSET = strtof(p.value.c_str(), NULL);
				}
				GLint VERTEX_OFFSET_UL = glGetUniformLocation(program->GetID(), "shaderBasicVertexInfo.offset");
				glUniform1f(VERTEX_OFFSET_UL, VERTEX_OFFSET);

				GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
				glUniform1i(shaderFeatureArray, shaderID);
			}

			else if (featureName == "INSTANCED_DRAW")
			{
				int shaderID = 4;

				ResolveCamera(program->GetID());

				IGLShader* sd = glShaders[SHADER_INDEX_VERTEX_INSTANCED];
				if (sd)
				{
					ModelDrawInfo modelDrawInfo;
					if (FindModelDrawInfoByMeshName(mesh->GetName(), modelDrawInfo))
					{
						char buf[100];
						sprintf_s(buf, 100, "vertexShaderInfo.shaderFeatureArray[%llu]", vertexShaderCount);
						GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
						glUniform1i(shaderFeatureArray, shaderID);
						//----------------------------------------------------------------------------------------

						InstancedVertexShader* shader = (InstancedVertexShader*)sd;
						shader->SetProgramID(program->GetID());
						shader->SetDrawInfo(modelDrawInfo);

						Parameter p;
						if (shaderFeature->parameters.FindParameterByName("MATRICES", p))
						{
							std::vector<Matrix4D> matrices = *(std::vector<Matrix4D>*)p.valuePtr;
							shader->SetWorldMatrices(matrices);
						}

						int renderMode = GL_FILL;
						if (shaderFeature->parameters.FindParameterByName("RENDER_MODE", p)) renderMode = strtol(p.value.c_str(), NULL, 0);
						shader->SetRenderMode(renderMode);

						int ENABLE_OFFSET_ALONG_NORMAL = 0;
						if (shaderFeature->parameters.FindParameterByName("ENABLE_OFFSET_ALONG_NORMAL", p)) ENABLE_OFFSET_ALONG_NORMAL = strtoul(p.value.c_str(), NULL, 0);
						shader->SetEnableOffsetAlongNormal(ENABLE_OFFSET_ALONG_NORMAL);

						float VERTEX_OFFSET = 0.0f;
						if (shaderFeature->parameters.FindParameterByName("VERTEX_OFFSET", p)) VERTEX_OFFSET = strtof(p.value.c_str(), NULL);
						shader->SetEnableOffsetAlongNormal(VERTEX_OFFSET);

						shader->Draw();
					}
				}
			}

			else if (featureName == "SKINNEDMESH")
			{
				//int shaderID = 3;
								
				ResolveCamera(program->GetID());

				glUseProgram(program->GetID());

				//char buf[100];
				//sprintf_s(buf, 100, "vertexShaderInfo.shaderFeatureArray[%llu]", vertexShaderCount);

				//GLint shaderFeatureArray = glGetUniformLocation(shaderProgramID, buf);
				//glUniform1i(shaderFeatureArray, shaderID);

				//ISkinnedMesh* smc = entity->GetComponent<ISkinnedMesh*>();
				//if (smc)
				//{
				//	// Copy all 100 bones to the shader
				//	GLint matBonesArray_UniLoc = glGetUniformLocation(shaderProgramID, "matrixBonesArray");
				//	// The "100" is to pass 100 values, starting at the pointer location of matBones[0];
				//	//glUniformMatrix4fv(matBonesArray_UniLoc, 100, GL_FALSE, glm::value_ptr(matBones[0]));

				//	std::vector<Matrix4D> currentFinalTransformation = smc->GetCurrentFinalTransformation();
				//	GLint numBonesUsed = (GLint)currentFinalTransformation.size();
				//	glUniformMatrix4fv(matBonesArray_UniLoc, numBonesUsed, GL_FALSE, glm::value_ptr(currentFinalTransformation[0]));
				//}
			}
			else
			{
				continue;
			}

			vertexShaderCount++;
		}

		GLint vertexShaderFeatureArrayCount = glGetUniformLocation(program->GetID(), "vertexShaderInfo.shaderFeatureArrayCount");
		glUniform1i(vertexShaderFeatureArrayCount, (GLint)vertexShaderCount);
	}
	else
	{
		SetLastErrorString("No shader features found!!!");
	}
}

void GameSystemVideo::WriteShaderFeatures(Entity*& entity, IMesh*& mesh, IMeshComponent*& meshComponent)
{
	IRender* vr = entity->GetComponent<IRender*>();
	if (!vr)
	{
		return;
	}
	
	std::vector<ShaderFeature*> shaderFeatures;
	if (vr->GetShaderFeatures(shaderFeatures))
	{
		IShaderProgram* program = this->shaderContainer->FindShaderProgramByAllFeatures(shaderFeatures);
		if (!program) return;

		std::string featureName;

		/*FRAGMENT SHADER*/
		size_t fragmentShaderCount = 0;
		for (size_t i = 0; i < shaderFeatures.size(); i++)
		{
			ShaderFeature* shaderFeature = shaderFeatures[i];
			if (shaderFeature->GetShaderType() != "fragment" || !shaderFeature->IsActive())
			{
				continue;
			}

			featureName = shaderFeature->GetName();

			if (featureName == "DISCARD_BY_HEIGHT") //ShaderID = 1
			{
				int shaderID = 1;

				glUseProgram(program->GetID());

				char buf[100];
				sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]",  fragmentShaderCount);

				GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
				glUniform1i(shaderFeatureArray, shaderID);

				Parameter p;
				bool rc = shaderFeature->parameters.FindParameterByName("HEIGHT_THRESHOLD", p);
				if (rc)
				{
					float HEIGHT_THRESHOLD_VALUE = (float)atof(p.value.c_str());

					GLint discardByHeightThreshold = glGetUniformLocation(program->GetID(), "discardByHeightThreshold");
					glUniform1f(discardByHeightThreshold, HEIGHT_THRESHOLD_VALUE);
				}
			}
			else if (featureName == "TEXTURIZE_BY_HEIGHT") //ShaderID = 2
			{
				//int shaderID = 2;

				glUseProgram(program->GetID());

				////FeatureArray
				//char buf[100];
				//sprintf_s(buf, 100, "shaderFeatureArray[%d]", i);
				//GLint shaderFeatureArray = glGetUniformLocation(shaderProgramID, buf);
				//glUniform1i(shaderFeatureArray, shaderID);

				////ModelHeight
				//std::string colliderType = gameObject->collider->GetType();

				//if (colliderType == "Sphere")
				//{
				//	float height = ((ColliderSphere*)(gameObject->collider))->radius;

				//	GLint modelHeight = glGetUniformLocation(shaderProgramID, "modelHeight");
				//	glUniform1f(modelHeight, height);
				//}
				//else if (colliderType == "AABB")
				//{
				//	float height = ((ColliderParallelepiped*)(gameObject->collider))->max.y;

				//	GLint modelHeight = glGetUniformLocation(shaderProgramID, "modelHeight");
				//	glUniform1f(modelHeight, height);
				//}
				//else if (colliderType == "Plane")
				//{
		
				//}
				//else if (colliderType == "Parallelepiped")
				//{

				//}
				///*else if (colliderType == "Capsule")
				//{
				//
				//}*/

				////Texture Ratios
				//for (Mesh* mesh : gameObject->meshes)
				//{
				//	for (unsigned int k = 0; k <mesh->textures.size(); k++)
				//	{
				//		sprintf_s(buf, 100, "TEXTURE_%d_HEIGHTRATIO", k);

				//		auto it = shaderFeature->parameters.find(buf);
				//		if (it != shaderFeature->parameters.end())
				//		{
				//			float HEIGHT_RATIO = (float)atof((it->second).value.c_str());

				//			sprintf_s(buf, 100, "textureRatioArray[%d]", k);

				//			GLint textureRatioArray = glGetUniformLocation(shaderProgramID, buf);
				//			glUniform1f(textureRatioArray, HEIGHT_RATIO);
				//		}
				//	}
				//}
			}
			else if (featureName == "TEXTURIZE") //ShaderID = 6
			{
				int shaderID = 6;
							
				IGLShader* sd = glShaders[SHADER_INDEX_FRAGMENT_TEXTURIZE];
				if (sd)
				{
					char buf[100];
					sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);
					GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
					glUniform1i(shaderFeatureArray, shaderID);
					//---------------------------------------------------------------------------------------

					TexturizeFragmentShader* shader = (TexturizeFragmentShader*)sd;
					shader->SetProgramID(program->GetID());
					shader->SetDiffuse(vr->GetDiffuse());
					shader->SetSpecular(vr->GetSpecular());

					std::vector<ITexture*> textures;
					meshComponent->GetMeshTextures(mesh, textures);
					shader->SetTextures(textures);

					IRigidbody* rb = entity->GetComponent<IRigidbody*>();
					/*Check for light components. These are lights that are tied to the entitites and will be transformed relative to it.*/
					std::vector<ILight*> all_lights, entity_lights, scene_lights;
					auto sceneLights_ptr = scene->GetLights();
					if (sceneLights_ptr)
					{
						scene_lights = *sceneLights_ptr;
						for (auto& l : scene_lights) all_lights.push_back(l);
					}
					if (entity->GetComponents(entity_lights))
					{
						/*There is only one light shader. This offsets the light index in the shader so the lights do not overlap.*/
						size_t lt_Sz = entity_lights.size();
						Vector3D currPosition = Vector3D();
						for (size_t i = 0; i < lt_Sz; i++)
						{
							/*Don't draw lights that are invisible.*/
							if (entity_lights[i]->IsVisible())
							{
								currPosition = rb->GetPosition();
								entity_lights[i]->SetPosition(currPosition);
								all_lights.push_back(entity_lights[i]);
							}
						}
					}
					
					shader->SetLights(all_lights);

					Parameter p;
					int TRANSPARENCY = 0;
					if (shaderFeature->parameters.FindParameterByName("TRANSPARENCY", p))TRANSPARENCY = strtoul(p.value.c_str(), NULL, 0);
					shader->SetTransparency(TRANSPARENCY);

					unsigned long offsetType = 0ull; /*0 - Fixed, 1 - Constant, 2 - Over time*/
					if (shaderFeature->parameters.FindParameterByName("OFFSET_TYPE", p)) offsetType = strtoul(p.value.c_str(), NULL, 0);

					Vector2D uvoffset = Vector2D(0.0f, 0.0f);
					if (shaderFeature->parameters.FindParameterByName("UVOFFSET", p)) uvoffset = StringHelpers::StrToVec2(p.value);

					switch (offsetType)
					{
						/*Offset is constant*/
					case 1:
					{
						/*Check for an internal parameter in the shaderfeature. This is where the last uvoffsets will be stored.*/
						if (!shaderFeature->parameters.HasParameter("LASTUVOFFSET"))
						{
							/*Updates offfset*/
							uvoffset += uvoffset;
							/*Does not exist. Create variable and store in the container.*/
							shaderFeature->parameters.Add("Vector2D", "LASTUVOFFSET", StringHelpers::Vec2ToStr(uvoffset), true);
						}
						else
						{
							/*Exists, replaces the old value with the current one.*/
							if (shaderFeature->parameters.FindParameterByName("LASTUVOFFSET", p))
							{
								Vector2D lastOffset = StringHelpers::StrToVec2(p.value);
								uvoffset = lastOffset;

								/*Updates offfset*/
								uvoffset += uvoffset;

								shaderFeature->parameters.SetValue("LASTUVOFFSET", StringHelpers::Vec2ToStr(uvoffset));
							}
							else { /*Not possible!*/ }
						}
					}
					break;
					/*Offset changes over time*/
					case 2:
					{
						/*Check for an internal parameter in the shaderfeature. This is where the last uvoffsets will be stored.*/
						if (!shaderFeature->parameters.HasParameter("LASTUVOFFSET"))
						{
							/*Updates offfset*/
							uvoffset += uvoffset * (float)frameTime.GetDeltaTime();
							/*Does not exist. Create variable and store in the container.*/
							shaderFeature->parameters.Add("Vector2D", "LASTUVOFFSET", StringHelpers::Vec2ToStr(uvoffset), true);
						}
						else
						{
							/*Exists, replaces the old value with the current one.*/
							if (shaderFeature->parameters.FindParameterByName("LASTUVOFFSET", p))
							{
								Vector2D lastOffset = StringHelpers::StrToVec2(p.value);
								
								/*Updates offfset*/
								uvoffset = lastOffset + uvoffset * (float)frameTime.GetDeltaTime();

								shaderFeature->parameters.SetValue("LASTUVOFFSET", StringHelpers::Vec2ToStr(uvoffset));
							}
							else { /*Not possible!*/ }
						}
					}
					break;
					case 0:
					default:
					{

					}
					break;
					}

					/*Sets the offset in the shader.*/
					shader->SetUVOffset(uvoffset);

					Vector2D tiling = Vector2D(1.0f, 1.0f);
					if (shaderFeature->parameters.FindParameterByName("TILING", p))
					{
						tiling = StringHelpers::StrToVec2(p.value);
						if (tiling.x == 0.0f) tiling.x = 1.0f;
						if (tiling.y == 0.0f) tiling.y = 1.0f;
					}
					shader->SetTiling(tiling);
				}
			}
			else if (featureName == "TEXTURIZE_SKYBOX") //ShaderID = 7
			{
				int shaderID = 7;
							
				IGLShader* sd = glShaders[SHADER_INDEX_FRAGMENT_TEXTURIZESKYBOX];
				if (sd)
				{
					char buf[100];
					sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);
					GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
					glUniform1i(shaderFeatureArray, shaderID);

					//---------------------------------------------------------------------------------------

					TexturizeSkyboxFragmentShader* shader = (TexturizeSkyboxFragmentShader*)sd;
					shader->SetProgramID(program->GetID());

					std::vector<ITexture*> textures;
					if (meshComponent->GetMeshTextures(mesh, textures)) 
					{
						shader->SetSkyboxTexture(textures[0]);
					}
				}
			}
			else if (featureName == "COLORIZE") //ShaderID = 9
			{
				int shaderID = 9;

				IGLShader* sd = glShaders[SHADER_INDEX_FRAGMENT_COLORIZE];
				if (sd)
				{
					char buf[100];
					sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);
					GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
					glUniform1i(shaderFeatureArray, shaderID);
					//---------------------------------------------------------------------------------------

					ColorizeFragmentShader* shader = (ColorizeFragmentShader*)sd;
					shader->SetProgramID(program->GetID());
					shader->SetDiffuse(vr->GetDiffuse());
					shader->SetSpecular(vr->GetSpecular());		

					IRigidbody* rb = entity->GetComponent<IRigidbody*>();
					/*Check for light components. These are lights that are tied to the entitites and will be transformed relative to it.*/
					std::vector<ILight*> all_lights, entity_lights, scene_lights;
					auto sceneLights_ptr = scene->GetLights();
					if (sceneLights_ptr)
					{
						scene_lights = *sceneLights_ptr;
						for (auto& l : scene_lights) all_lights.push_back(l);
					}
					if (entity->GetComponents(entity_lights))
					{
						/*There is only one light shader. This offsets the light index in the shader so the lights do not overlap.*/
						size_t lt_Sz = entity_lights.size();
						Vector3D currPosition = Vector3D();
						for (size_t i = 0; i < lt_Sz; i++)
						{
							/*Don't draw lights that are invisible.*/
							if (entity_lights[i]->IsVisible())
							{
								currPosition = rb->GetPosition();
								entity_lights[i]->SetPosition(currPosition);
								all_lights.push_back(entity_lights[i]);
							}
						}
					}

					shader->SetLights(all_lights);

					Parameter p;
					unsigned int isLit = 1u;
					if (shaderFeature->parameters.FindParameterByName("IsLit", p)) isLit = strtoul(p.value.c_str(), NULL, 0);
					shader->SetIsLit(isLit != 0u);
				}
			}
			else if (featureName == "IMPOSTER") //ShaderID = 10
			{
				int shaderID = 10;
								
				IGLShader* sd = glShaders[SHADER_INDEX_FRAGMENT_IMPOSTER];
				if (sd)
				{
					char buf[100];
					sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);
					GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
					glUniform1i(shaderFeatureArray, shaderID);
					//---------------------------------------------------------------------------------------

					ImposterFragmentShader* shader = (ImposterFragmentShader*)sd;
					shader->SetProgramID(program->GetID());
					shader->SetDiffuse(vr->GetDiffuse());
					shader->SetSpecular(vr->GetSpecular());

					IRigidbody* rb = entity->GetComponent<IRigidbody*>();
					/*Check for light components. These are lights that are tied to the entitites and will be transformed relative to it.*/
					std::vector<ILight*> all_lights, entity_lights, scene_lights;
					auto sceneLights_ptr = scene->GetLights();
					if (sceneLights_ptr)
					{
						scene_lights = *sceneLights_ptr;
						for (auto& l : scene_lights) all_lights.push_back(l);
					}
					if (entity->GetComponents(entity_lights))
					{
						/*There is only one light shader. This offsets the light index in the shader so the lights do not overlap.*/
						size_t lt_Sz = entity_lights.size();
						Vector3D currPosition = Vector3D();
						for (size_t i = 0; i < lt_Sz; i++)
						{
							/*Don't draw lights that are invisible.*/
							if (entity_lights[i]->IsVisible())
							{
								currPosition = rb->GetPosition();
								entity_lights[i]->SetPosition(currPosition);
								all_lights.push_back(entity_lights[i]);
							}
						}
					}

					shader->SetLights(all_lights);

					std::vector<ITexture*> textures;
					meshComponent->GetMeshTextures(mesh, textures);
					shader->SetTextures(textures);

					Parameter p;
					int TRANSPARENCY = 0;
					if (shaderFeature->parameters.FindParameterByName("TRANSPARENCY", p))TRANSPARENCY = strtoul(p.value.c_str(), NULL, 0);
					shader->SetTransparency(TRANSPARENCY);

					Vector2D uvoffset = Vector2D(0.0f, 0.0f);
					if (shaderFeature->parameters.FindParameterByName("UVOFFSET", p)) uvoffset = StringHelpers::StrToVec2(p.value);
					shader->SetUVOffset(uvoffset);

					Vector2D tiling = Vector2D(1.0f, 1.0f);
					if (shaderFeature->parameters.FindParameterByName("TILING", p))
					{
						tiling = StringHelpers::StrToVec2(p.value);
						if (tiling.x == 0.0f) tiling.x = 1.0f;
						if (tiling.y == 0.0f) tiling.y = 1.0f;
					}
					shader->SetTiling(tiling);
				}
			}
			else if (featureName == "REFLECTREFRACT") //ShaderID = 11
			{
				int shaderID = 11;
								
				glUseProgram(program->GetID());

				char buf[100];
				sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]",  fragmentShaderCount);

				GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
				glUniform1i(shaderFeatureArray, shaderID);
			}
			else if (featureName == "USE_HEIGHTMAP") //ShaderID = 14
			{
				int shaderID = 14;
							
				glUseProgram(program->GetID());

				char buf[100];
				sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]",  fragmentShaderCount);

				GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
				glUniform1i(shaderFeatureArray, shaderID);

				Vector2D offsetOverTime = Vector2D();
				Parameter p;
				bool rc = shaderFeature->parameters.FindParameterByName("OFFSET_X_OVER_TIME", p);
				if (rc)
				{
					offsetOverTime.x = (float)atof(p.value.c_str());
				}
				rc = shaderFeature->parameters.FindParameterByName("OFFSET_Y_OVER_TIME", p);
				if (rc)
				{
					offsetOverTime.y = (float)atof(p.value.c_str());
				}
				rc = shaderFeature->parameters.FindParameterByName("OFFSET_X_OVER_TIME_VAR", p);
				if (rc)
				{
					float lastOffsetOverTimeX = (float)atof(p.value.c_str());
					lastOffsetOverTimeX += offsetOverTime.x * frameTime.GetDeltaTime();
					offsetOverTime.x = lastOffsetOverTimeX;

					shaderFeature->parameters.SetValue(p.name, std::to_string(lastOffsetOverTimeX));
				}
				rc = shaderFeature->parameters.FindParameterByName("OFFSET_Y_OVER_TIME_VAR", p);
				if (rc)
				{
					float lastOffsetOverTimeY = (float)atof(p.value.c_str());
					lastOffsetOverTimeY += offsetOverTime.y * frameTime.GetDeltaTime();
					offsetOverTime.y = lastOffsetOverTimeY;

					shaderFeature->parameters.SetValue(p.name, std::to_string(lastOffsetOverTimeY));
				}

				GLint textureOffsetVec = glGetUniformLocation(program->GetID(), "textureOffsetVec");
				glUniform2f(textureOffsetVec, offsetOverTime.x, offsetOverTime.y);

				rc = shaderFeature->parameters.FindParameterByName("TEXTURE_ID", p);
				if (rc)
				{
					int TEXTURE_ID = atoi(p.value.c_str());

					GLint textureID = glGetUniformLocation(program->GetID(), "textureID");
					glUniform1i(textureID, TEXTURE_ID);
				}
			}
			else if (featureName == "INTERPOLATION_COLORIZE")
			{
				int shaderID = 15;

				IGLShader* sd = glShaders[SHADER_INDEX_FRAGMENT_INTERPOLATION_COLORIZE];
				if (sd)
				{
					char buf[100];
					sprintf_s(buf, 100, "fragmentShaderInfo.shaderFeatureArray[%llu]", fragmentShaderCount);
					GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
					glUniform1i(shaderFeatureArray, shaderID);
					//---------------------------------------------------------------------------------------

					InterpolationColorizeFragmentShader* shader = (InterpolationColorizeFragmentShader*)sd;
					shader->SetProgramID(program->GetID());

					shader->SetDeltaTime(this->frameTime.GetDeltaTime());
					shader->SetRunTime(this->frameTime.GetRunTime());
					shader->SetSpecular(vr->GetSpecular());

					IRigidbody* rb = entity->GetComponent<IRigidbody*>();
					/*Check for light components. These are lights that are tied to the entitites and will be transformed relative to it.*/
					std::vector<ILight*> all_lights, entity_lights, scene_lights;
					auto sceneLights_ptr = scene->GetLights();
					if (sceneLights_ptr)
					{
						scene_lights = *sceneLights_ptr;
						for (auto& l : scene_lights) all_lights.push_back(l);
					}
					if (entity->GetComponents(entity_lights))
					{
						/*There is only one light shader. This offsets the light index in the shader so the lights do not overlap.*/
						size_t lt_Sz = entity_lights.size();
						Vector3D currPosition = Vector3D();
						for (size_t i = 0; i < lt_Sz; i++)
						{
							/*Don't draw lights that are invisible.*/
							if (entity_lights[i]->IsVisible())
							{
								currPosition = rb->GetPosition();
								entity_lights[i]->SetPosition(currPosition);
								all_lights.push_back(entity_lights[i]);
							}
						}
					}

					shader->SetLights(all_lights);

					Parameter p;
					Vector4D colorA = Vector4D();
					if (shaderFeature->parameters.FindParameterByName("COLOR_A", p)) colorA = StringHelpers::StrToVec4(p.value);
					shader->SetColorA(colorA);

					Vector4D colorB = Vector4D();
					if (shaderFeature->parameters.FindParameterByName("COLOR_B", p)) colorB = StringHelpers::StrToVec4(p.value);
					shader->SetColorB(colorB);
				}
			}
			else
			{
				continue;
			}

			fragmentShaderCount++;
		}

		GLint shaderFeatureArrayCount = glGetUniformLocation(program->GetID(), "fragmentShaderInfo.shaderFeatureArrayCount");
		glUniform1i(shaderFeatureArrayCount, (GLint)fragmentShaderCount);

		/*VERTEX SHADER*/
		size_t vertexShaderCount = 0;		
		for (size_t i = 0; i < shaderFeatures.size(); i++)
		{
			ShaderFeature* shaderFeature = shaderFeatures[i];
			if (shaderFeature->GetShaderType() != "vertex")
			{
				continue;
			}

			featureName = shaderFeature->GetName();

			if  (featureName == "BASIC_VERTEX_DRAW")
			{
				int shaderID = 1;

				ResolveCamera(program->GetID());

				IGLShader* sd = glShaders[SHADER_INDEX_VERTEX_BASIC];
				if (sd)
				{
					ModelDrawInfo modelDrawInfo;
					if (FindModelDrawInfoByMeshName(mesh->GetName(), modelDrawInfo))
					{
						char buf[100];
						sprintf_s(buf, 100, "vertexShaderInfo.shaderFeatureArray[%llu]", vertexShaderCount);
						GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
						glUniform1i(shaderFeatureArray, shaderID);
						//----------------------------------------------------------------------------------//

						BasicVertexShader* shader = (BasicVertexShader*)sd;
						shader->SetProgramID(program->GetID());
						shader->SetDrawInfo(modelDrawInfo);
						shader->SetRenderMode(vr->GetRenderMode());

						Matrix4D matrixModel(1.0f);
						IRigidbody* rb = entity->GetComponent<IRigidbody*>();
						if (rb) matrixModel = rb->GetWorldMatrix();
						shader->SetModelMatrix(matrixModel);

						Parameter p;
						int enable_offset_along_normal = 0;
						if (shaderFeature->parameters.FindParameterByName("ENABLE_OFFSET_ALONG_NORMAL", p)) enable_offset_along_normal = strtoul(p.value.c_str(), NULL, 0);
						shader->SetEnableOffsetAlongNormal(enable_offset_along_normal);

						float vertex_offset = 0.0f;
						if (enable_offset_along_normal)
						{
							if (shaderFeature->parameters.FindParameterByName("VERTEX_OFFSET", p)) vertex_offset = strtof(p.value.c_str(), NULL);

							/*Check for an internal parameter in the shaderfeature. This is where the last offsets will be stored.*/
							if (!shaderFeature->parameters.HasParameter("LASTVERTEXOFFSET"))
							{
								/*Updates offfset*/
								vertex_offset += vertex_offset * (float)frameTime.GetDeltaTime();
								/*Does not exist. Create variable and store in the container.*/
								shaderFeature->parameters.Add("Vector2D", "LASTVERTEXOFFSET", std::to_string(vertex_offset), true);
							}
							else
							{
								/*Exists, replaces the old value with the current one.*/
								if (shaderFeature->parameters.FindParameterByName("LASTVERTEXOFFSET", p))
								{
									float lastOffset = strtof(p.value.c_str(), NULL);

									/*Updates offfset*/
									vertex_offset = lastOffset + vertex_offset * (float)frameTime.GetDeltaTime();

									shaderFeature->parameters.SetValue("LASTVERTEXOFFSET", std::to_string(vertex_offset));
								}
								else { /*Not possible!*/ }
							}
						}						
						shader->SetVertexOffset(vertex_offset);
					
						shader->Draw();
					}
				}
			}

			else if (featureName == "SKINNEDMESH")
			{
				int shaderID = 3;

				ResolveCamera(program->GetID());

				IGLShader* sd = glShaders[SHADER_INDEX_VERTEX_SKINNEDMESH];
				if (sd)
				{
					ModelDrawInfo modelDrawInfo;
					if (FindModelDrawInfoByMeshName(mesh->GetName(), modelDrawInfo))
					{
						char buf[100];
						sprintf_s(buf, 100, "vertexShaderInfo.shaderFeatureArray[%llu]", vertexShaderCount);
						GLint shaderFeatureArray = glGetUniformLocation(program->GetID(), buf);
						glUniform1i(shaderFeatureArray, shaderID);
						//----------------------------------------------------------------------------------//

						SkinnedMeshVertexShader* shader = (SkinnedMeshVertexShader*)sd;
						shader->SetProgramID(program->GetID());
						shader->SetDrawInfo(modelDrawInfo);
						shader->SetRenderMode(vr->GetRenderMode());

						Matrix4D matrixModel(1.0f);
						IRigidbody* rb = entity->GetComponent<IRigidbody*>();
						if (rb) matrixModel = rb->GetWorldMatrix();
						shader->SetModelMatrix(matrixModel);

						ISkinnedMesh* smc = entity->GetComponent<ISkinnedMesh*>();
						std::vector<Matrix4D> currentFinalTransformation;
						if (smc)
						{
							currentFinalTransformation = smc->GetCurrentFinalTransformation();
						}
						shader->SetBoneMatrices(currentFinalTransformation);

						Parameter p;
						int enable_offset_along_normal = 0;
						if (shaderFeature->parameters.FindParameterByName("ENABLE_OFFSET_ALONG_NORMAL", p)) enable_offset_along_normal = strtoul(p.value.c_str(), NULL, 0);
						shader->SetEnableOffsetAlongNormal(enable_offset_along_normal);

						float vertex_offset = 0.0f;
						if (shaderFeature->parameters.FindParameterByName("VERTEX_OFFSET", p)) vertex_offset = strtof(p.value.c_str(), NULL);
						shader->SetVertexOffset(vertex_offset);

						shader->Draw();
					}
				}
			}
			
			else
			{
				continue;
			}

			vertexShaderCount++;
		}

		GLint vertexShaderFeatureArrayCount = glGetUniformLocation(program->GetID(), "vertexShaderInfo.shaderFeatureArrayCount");
		glUniform1i(vertexShaderFeatureArrayCount, (GLint)vertexShaderCount);
	}
	else
	{
		SetLastErrorString("No shader features found!!!");
	}
}

void GameSystemVideo::DrawParticles(Entity*& entity)
{
	IRigidbody* rb = entity->GetComponent<IRigidbody*>();

	IParticleEmitter* particleEmitter = entity->GetComponent<IParticleEmitter*>();
	if (!particleEmitter || !particleEmitter->IsActive()) return;
	particleEmitter->SetPosition(rb->GetPosition());
	particleEmitter->Update(this->frameTime);

	IRender* renderComponent = entity->GetComponent<IRender*>();
	if (!renderComponent) return;


	ModelDrawInfo info;
	std::string meshName;
	std::vector<IParticleMeshComponent*> mCs;
	std::vector<IMesh*> meshes;
	std::vector<IParticleEmitter::Particle*> particles;
	if (entity->GetComponents(mCs))
	{
		for (auto& pMC : mCs)
		{
			if (pMC->GetMeshes(meshes))
			{				
				if (particleEmitter->GetParticles(particles))
				{
					std::vector<Matrix4D> matrices;
					size_t particles_sz = particles.size();
					matrices.resize(particles_sz);
					for (size_t i = 0; i < particles_sz; i++)
					{
						/*If particle is imposter, use the camera eye to "rotate"*/
						if (particles[i]->lockToCamera)
						{
							matrices[i] = particles[i]->GetWorldMatrix(scene->GetCamera()->GetEyePosition());
						}
						else
						{
							matrices[i] = particles[i]->GetWorldMatrix();
						}
					}
					std::vector<ShaderFeature*> shaderFeatures;
					if (particleEmitter->GetShaderFeatures(shaderFeatures))
					{
						for (auto& sf : shaderFeatures)
						{
							if (sf->GetName() == "INSTANCED_DRAW")
							{
								if (sf->parameters.HasParameter("MATRICES"))
								{
									sf->parameters.SetValue("MATRICES", &matrices);
								}
								else
								{
									sf->parameters.Add("std::vector<Matrix4D>*", "MATRICES", &matrices, true);
								}

								break;
							}
						}
					}

					auto mc = (IMeshComponent*)pMC;
					for (auto& mesh : meshes)
					{
						WriteShaderFeatures(shaderFeatures, mesh, mc);
					}
				}
			}
		}
	}
}

GameSystemVideo::GameSystemVideo() : SystemVideo()
{
	this->isUpdateComplete = false;
	this->isActive = true;
	CreateGLShaders();
}

const bool GameSystemVideo::ResolveCamera(const unsigned int& programID)
{
	/*If the scene is being drawn to a framebuffer, then use the camera specified by the buffer*/
	ICamera* camera = nullptr;

	/*Draws a scene to an FBO.*/
	FBO* destination = nullptr;
	for (std::string fboName : *scene->GetFrameBufferNames())
	{
		/*Checks if there is a buffer with that name*/
		auto it = std::find_if(graphicBufferReferenceMap.begin(), graphicBufferReferenceMap.end(), [fboName](std::pair<std::string, IGraphicsBuffer*> p)
			{
				return p.second->GetType() == "FBO" && ((FBO*)p.second)->GetName() == fboName;
			});
		if (it != graphicBufferReferenceMap.end())
		{
			destination = (FBO*)(it->second);
		}
	}

	if (destination && destination->GetType() == "FBO" && !destination->GetCameraReference().empty())
	{
		/*Checks if there is a pointer to the camera in the global map of cameras.*/
		auto camIT = cameraReferenceMap.find(destination->GetCameraReference());
		if (camIT != cameraReferenceMap.end())
		{
			camera = camIT->second;
		}
		/*Use the camera from the current scene if not.*/
		else
		{
			camera = scene->GetCamera();
		}
	}
	else
	{
		camera = scene->GetCamera();
	}

	if (!camera)
	{
		SetLastErrorString("The scene has no camera!");
		return false;
	}
	UpdateCamera(camera, programID);
}

void GameSystemVideo::CreateGLShaders()
{
	glShaders.resize(30);

	IGLShader* shader = nullptr;

	/*VERTEX SHADERS*/
	shader = new BasicVertexShader();
	shader->SetName("BASIC_VERTEX_DRAW");
	glShaders[SHADER_INDEX_VERTEX_BASIC] = shader;

	shader = new InstancedVertexShader();
	shader->SetName("INSTANCED_DRAW");
	glShaders[SHADER_INDEX_VERTEX_INSTANCED] = shader;

	shader = new SkinnedMeshVertexShader();
	shader->SetName("SKINNEDMESH");
	glShaders[SHADER_INDEX_VERTEX_SKINNEDMESH] = shader;

	/*FRAGMENT SHADERS*/
	shader = new ColorizeFragmentShader();
	shader->SetName("COLORIZE");
	glShaders[SHADER_INDEX_FRAGMENT_COLORIZE] = shader;

	shader = new TexturizeFragmentShader();
	shader->SetName("TEXTURIZE");
	glShaders[SHADER_INDEX_FRAGMENT_TEXTURIZE] = shader;

	shader = new TexturizeSkyboxFragmentShader();
	shader->SetName("TEXTURIZE_SKYBOX");
	glShaders[SHADER_INDEX_FRAGMENT_TEXTURIZESKYBOX] = shader;

	shader = new InterpolationColorizeFragmentShader();
	shader->SetName("INTERPOLATION_COLORIZE");
	glShaders[SHADER_INDEX_FRAGMENT_INTERPOLATION_COLORIZE] = shader;

	shader = new ImposterFragmentShader();
	shader->SetName("IMPOSTER");
	glShaders[SHADER_INDEX_FRAGMENT_IMPOSTER] = shader;

	shader = new ImposterFragmentShaderV2();
	shader->SetName("IMPOSTERV2");
	glShaders[SHADER_INDEX_FRAGMENT_IMPOSTER_V2] = shader;
}

GameSystemVideo::~GameSystemVideo()
{
	Dispose();
}

void GameSystemVideo::SetSystemFlags(const unsigned long long& flags)
{
	this->systemFlags.store(flags);
}

void GameSystemVideo::SetShaderContainer(IShaderContainer*& value)
{
	this->shaderContainer = value;
}

IShaderContainer* GameSystemVideo::GetShaderContainer()
{
	return this->shaderContainer;
}

const bool GameSystemVideo::IsUpdateComplete()
{
	return this->isUpdateComplete.load();
}

const bool GameSystemVideo::IsActive()
{
	return this->isActive.load();
}

void GameSystemVideo::Activate(const bool& value)
{
	this->isActive.store(value);
}

const bool GameSystemVideo::CreateModelDrawInfo(IMesh*& mesh, ModelDrawInfo& modelDrawInfo)
{
	modelDrawInfo = ModelDrawInfo();

	modelDrawInfo.name = mesh->GetName();
	modelDrawInfo.path = mesh->GetPath();

	std::vector<Vertex3D*> vertices;
	if (mesh->GetVertices(vertices))
	{
		size_t sz = vertices.size();
		modelDrawInfo.vertices3D.reserve(sz);

		for (unsigned int i = 0; i < sz; i++)
		{
			modelDrawInfo.vertices3D.push_back(*vertices[i]);
		}
	}	

	// Allocate the index array
	std::vector<Triangle3D*> triangles;
	if (mesh->GetTriangles(triangles))
	{
		size_t sz = triangles.size();
		modelDrawInfo.indices.reserve(3 * sz);

		for (unsigned int i = 0; i < triangles.size(); i++)
		{
			modelDrawInfo.indices.push_back(triangles[i]->pointA.index);
			modelDrawInfo.indices.push_back(triangles[i]->pointB.index);
			modelDrawInfo.indices.push_back(triangles[i]->pointC.index);
		}
	}

	return true;
}

const bool GameSystemVideo::LoadModelDrawInfoToVAO(ModelDrawInfo& modelDrawInfo)
{
	//Create a VAO (Vertex Array Object), which will keep track of all the 'state' needed to draw from this buffer
		//Generates an new id for the buffer
	glGenVertexArrays(1, &(modelDrawInfo.VAO_ID));
	//This function binds the buffer, sets this buffer as the VAO
	glBindVertexArray(modelDrawInfo.VAO_ID);

	//Now ANY state that is related to vertex or index buffer
	//and vertex attribute layout, is stored in the 'state' 
	//of the VAO... 

	glGenBuffers(1, &(modelDrawInfo.vertexBufferID));
	glBindBuffer(GL_ARRAY_BUFFER, modelDrawInfo.vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3D) * modelDrawInfo.vertices3D.size(), (GLvoid*)modelDrawInfo.vertices3D.data(), /*GL_STATIC_DRAW*/ GL_DYNAMIC_DRAW);

	//Copy the index buffer into the video card, too
	//Create an index buffer.
	glGenBuffers(1, &(modelDrawInfo.indexBufferID));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelDrawInfo.indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * modelDrawInfo.indices.size(), (GLvoid*)modelDrawInfo.indices.data(), GL_STATIC_DRAW);

	const char vPOSITION[255] = "vertexPosition";
	const char vCOLOR[255] = "vertexColor";
	const char vBINORMAL[255] = "vertexBiNormal";
	const char vNORMAL[255] = "vertexNormal";
	const char vTEXTURE[255] = "vertexTexture";
	const char vTANGENT[255] = "vertexTangent";
	const char vBONEID[255] = "vertexBoneID";
	const char vBONEWEIGHT[255] = "vertexBoneWeight";
	std::map<std::string, GLint> uLMap;

	//Finds the position location, then sets the vertex attributes for this shader
	uLMap[vPOSITION] = SHADER_LOC_VS_POSITION;
	glEnableVertexAttribArray(uLMap[vPOSITION]);
	glVertexAttribPointer(uLMap[vPOSITION], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)(offsetof(Vertex3D, x)));

	//Finds the color location, then sets the vertex attributes for this shader
	uLMap[vCOLOR] = SHADER_LOC_VS_COLOR;
	glEnableVertexAttribArray(uLMap[vCOLOR]);
	glVertexAttribPointer(uLMap[vCOLOR], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)(offsetof(Vertex3D, r)));

	//Finds the normals location, then sets the vertex attributes for this shader
	uLMap[vNORMAL] = SHADER_LOC_VS_NORMAL;
	glEnableVertexAttribArray(uLMap[vNORMAL]);
	glVertexAttribPointer(uLMap[vNORMAL], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)(offsetof(Vertex3D, nx)));

	//Finds the texture location, then sets the vertex attributes for this shader
	uLMap[vTEXTURE] = SHADER_LOC_VS_TEXTURE;
	glEnableVertexAttribArray(uLMap[vTEXTURE]);
	glVertexAttribPointer(uLMap[vTEXTURE], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)(offsetof(Vertex3D, u0)));

	//Finds the tangents location, then sets the vertex attributes for this shader
	uLMap[vTANGENT] = SHADER_LOC_VS_TANGENT;	// program;
	glEnableVertexAttribArray(uLMap[vTANGENT]);
	glVertexAttribPointer(uLMap[vTANGENT], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)(offsetof(Vertex3D, tx)));

	//Finds the bumpmapping location, then sets the vertex attributes for this shader
	uLMap[vBINORMAL] = SHADER_LOC_VS_BINORMAL;
	glEnableVertexAttribArray(uLMap[vBINORMAL]);
	glVertexAttribPointer(uLMap[vBINORMAL], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)(offsetof(Vertex3D, bx)));

	//Finds the boneID location, then sets the vertex attributes for this shader
	uLMap[vBONEID] = SHADER_LOC_VS_BONEID;
	glEnableVertexAttribArray(uLMap[vBONEID]);
	glVertexAttribPointer(uLMap[vBONEID], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)(offsetof(Vertex3D, boneID[0])));

	//Finds the boneWeight location, then sets the vertex attributes for this shader
	uLMap[vBONEWEIGHT] = SHADER_LOC_VS_BONEWEIGHT;
	glEnableVertexAttribArray(uLMap[vBONEWEIGHT]);
	glVertexAttribPointer(uLMap[vBONEWEIGHT], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)(offsetof(Vertex3D, boneWeights[0])));

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	for (const auto& element : uLMap)
	{
		glDisableVertexAttribArray(element.second);
	}

	// Store the draw information into the map
	this->VAOMap[modelDrawInfo.name] = modelDrawInfo;

	return true;
}

void GameSystemVideo::Initialize()
{
	SetGLCallbacks();
}

void* GameSystemVideo::GetInstance()
{
	if (!systemVideo)
	{
		systemVideo = new GameSystemVideo();
	}

	return systemVideo;
}

void* GameSystemVideo::GetInstance(unsigned int numberOfArguments, ...)
{
	return GetInstance();
}

const bool GameSystemVideo::FindModelDrawInfoByMeshName(const std::string& name, ModelDrawInfo& modelDrawInfo)
{
	auto it = VAOMap.find(name);

	if (it != VAOMap.end())
	{
		modelDrawInfo = it->second;
		return true;
	}

	modelDrawInfo = ModelDrawInfo();
	return false;
}

void GameSystemVideo::DrawObject(Entity*& entity)
{
	/*Check for particle emitters and draw them on the screen.*/
	if (entity->HasComponent<IParticleEmitter*>()) DrawParticles(entity);

	/*A render component usually carries general drawing information of the entity, like color and which shader to use.*/
	IRender* renderComponent = entity->GetComponent<IRender*>();
	if (!renderComponent) return;
	
	/*In this engine, all objects have rigidbodies and the rigidbody has the transform.*/
	IRigidbody* rb = entity->GetComponent<IRigidbody*>();

	ModelDrawInfo info;
	std::string meshName;
	std::vector<IMeshComponent*> mCs;
	std::vector<IMesh*> meshes;
	if (entity->GetComponents(mCs))
	{
		/*Mesh components carry both texture and mesh informations.*/
		for (auto& mC : mCs)
		{
			if (mC->GetTypeID() != ENGINE_OBJ_ID_MSH_MESHCOMPONENT) continue;

			if (mC->GetMeshes(meshes))
			{
				for (auto& mesh : meshes)
				{				
					//Calls the shaders
					WriteShaderFeatures(entity, mesh, mC);					
				}
			}
		}
	}
}

void GameSystemVideo::SetupEntities(Entity*& entity)
{
	LoadTextures(entity);

	std::vector<IMeshComponent*> mCs;
	if (entity->GetComponents(mCs))
	{
		for (IMeshComponent*& mC : mCs)
		{
			std::vector<IMesh*> meshes;
			if (mC->GetMeshes(meshes))
			{
				ModelDrawInfo mdi = ModelDrawInfo();
				for (IMesh*& mesh : meshes)
				{
					CreateModelDrawInfo(mesh, mdi);
					LoadModelDrawInfoToVAO(mdi);
				}
			}
		}
	}

	std::vector<IParticleMeshComponent*> pcms;
	if (entity->GetComponents(pcms))
	{
		for (IParticleMeshComponent*& mC : pcms)
		{
			std::vector<IMesh*> meshes;
			if (mC->GetMeshes(meshes))
			{
				ModelDrawInfo mdi = ModelDrawInfo();
				for (IMesh*& mesh : meshes)
				{
					CreateModelDrawInfo(mesh, mdi);
					LoadModelDrawInfoToVAO(mdi);
				}
			}
		}
	}
}

void GameSystemVideo::SetupLevel(Level*& level)
{
	ModelDrawInfo mdi;
	for (Scene*& scene : *level->GetScenes())
	{
		std::vector<Entity*> entities;
		if (scene->GetEntities(entities))
		{
			for (Entity*& entity : entities)
			{
				auto vr = entity->GetComponent<IRender*>();
				if (vr)
				{
					std::vector<ShaderFeature*> sfs;
					if (vr->GetShaderFeatures(sfs))
					{
						systemVideo->SetupEntities(entity);

						/*Overwrite VAO if skinned animation is present*/
						std::vector<ISkinnedMesh*> skinnedMeshes;
						if (entity->GetComponents(skinnedMeshes))
						{
							for (ISkinnedMesh*& skm : skinnedMeshes)
							{
								skm->GetModelDrawInfo(mdi);
								systemVideo->LoadModelDrawInfoToVAO(mdi);
							}
						}						
					}
				}
			}
		}
	}
}

void GameSystemVideo::UpdateCamera(ICamera*& camera, const unsigned int& programID)
{
	//Updates camera
	camera->Update(this->frameTime);

	/*Protection for the AR*/
	if (std::isnan(aspectRatio)) return;

	//Projection matrix
	Matrix4D projectionMatrix = camera->GetProjectionMatrix(aspectRatio);

	//View matrix
	Matrix4D viewMatrix = camera->GetViewMatrix();

	//Set projection and view
	GLuint matrixView_location = glGetUniformLocation(programID, "matrixView");
	glUniformMatrix4fv(matrixView_location, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	GLuint matrixProjection_location = glGetUniformLocation(programID, "matrixProjection");
	glUniformMatrix4fv(matrixProjection_location, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	//Set Eye Location
	GLuint eye_location = glGetUniformLocation(programID, "eyeLocation");
	Vector3D eyePosition = camera->GetEyePosition();
	glUniform4f(eye_location, eyePosition.x, eyePosition.y, eyePosition.z, 1.0f);

	//Set clipping plane info	
	GLuint clippingPlaneInfo_location = glGetUniformLocation(programID, "clippingPlaneInfo");
	glUniform2f(clippingPlaneInfo_location, camera->GetNearClippingPlane(), camera->GetFarClippingPlane());
}

void GameSystemVideo::BindOnWindowResizeEvent(std::function<void(const unsigned int&, const unsigned int&)> fn)
{
	onWindowResizedEvent = fn;
}

void GameSystemVideo::DrawFBO(Entity*& entity, FBO*& framebuffer)
{
	glCullFace(GL_FRONT_AND_BACK);

	IShaderProgram* program = this->shaderContainer->FindShaderProgramByIndex(0);

	ModelDrawInfo info;
	std::vector<IMeshComponent*> mCs;
	std::vector<IMesh*> meshes;
	if (entity->GetComponents(mCs))
	{
		for (auto& mc : mCs)
		{
			if (mc->GetMeshes(meshes))
			{
				for (auto& mesh : meshes)
				{
					glActiveTexture(GL_TEXTURE26);
					glBindTexture(GL_TEXTURE_2D, framebuffer->colourTexture_0_ID);	// Texture now assoc with texture unit 0
					//glBindTexture(GL_TEXTURE_2D, framebuffer->depthTexture_ID);	// Texture now assoc with texture unit 0
					GLint fboTexture_ul = glGetUniformLocation(program->GetID(), "textureProcessInfo.fullscreenTexture");
					glUniform1i(fboTexture_ul, 26);	// Texture unit 40

					//Enable shader features
					WriteShaderFeatures(entity, mesh, mc);
				}
			}
		}
	}
}

void GameSystemVideo::DrawStencil(Entity*& entity, StencilBuffer* framebuffer)
{
	//glEnable(GL_STENCIL_TEST);
	//			glStencilOp(GL_KEEP,	// Stencil fails KEEP the original value (47)
	//				GL_KEEP,			// Depth fails KEEP the original value
	//				GL_REPLACE);		// Stencil AND depth PASSES, REPLACE with 133
	//glStencilFunc(GL_ALWAYS,			// If is succeed, ALWAYS do this
	//	133,							// Replace with this
	//	0xFF);							// Mask of 1111,1111 (no mask)

	////glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	//glDepthMask(GL_FALSE);

	//IRender* renderComponent = entity->GetComponent<IRender*>();

	//std::vector<IMeshComponent*> mCs;
	//if (entity->GetComponents(mCs))
	//{
	//	for (IMeshComponent* mc : mCs)
	//	{
	//		std::vector<IMesh*> meshes;
	//		if (mc->GetMeshes(meshes))
	//		{
	//			for (IMesh* mesh : meshes)
	//			{
	//				//Find the VAO for the mesh and push to the GPU
	//				std::map< std::string, ModelDrawInfo>::iterator it = VAOMap.find(mesh->GetName());
	//				if (it != VAOMap.end())
	//				{
	//					ModelDrawInfo info = VAOMap[mesh->GetName()];

	//					//Draw textures
	//					if (renderComponent->HasShaderFeature("TEXTURIZE") || renderComponent->HasShaderFeature("TEXTURIZE_SKYBOX"))
	//					{
	//						std::vector<ITexture*> textures;
	//						if (mesh->GetTextures(textures))
	//						{
	//							if (entity->GetTypeID() == ENGINE_OBJ_ID_SCN_GAMEOBJECT)
	//							{
	//								//Tells the shader how many textures this gameobject supports
	//								GLint textureCount = glGetUniformLocation(shaderProgramID, "textureProcessInfo.textureCount");
	//								glUniform1i(textureCount, (GLint)textures.size());

	//								//Tells the shader what's the first texture for this mesh
	//								int startTextureIndex = GetLoadedTextureIndex(textures[0]->path);
	//								GLint textureIndex = glGetUniformLocation(shaderProgramID, "textureIndex");
	//								glUniform1i(textureIndex, startTextureIndex);

	//								//Textures
	//								for (unsigned int i = 0; i < textures.size(); i++)
	//								{
	//									ITexture* texture = textures[i];

	//									int currentTexture = GetLoadedTextureIndex(texture->path);

	//									glActiveTexture(GL_TEXTURE0 + currentTexture);			// Texture Unit x, max is 32
	//									glBindTexture(GL_TEXTURE_2D, texture->GetID());				// Texture now assoc with texture unit 0

	//									char textureSampleName[100];
	//									sprintf_s(textureSampleName, 100, "%s[%d]", "textureProcessInfo.textureSample", i /*currentTexture*/);
	//									GLint textSamp_UL = glGetUniformLocation(shaderProgramID, textureSampleName);
	//									glUniform1i(textSamp_UL, currentTexture);
	//								}
	//								//------------------------------------------------------------------------

	//								glCullFace(GL_BACK);
	//							}
	//							else if (entity->GetTypeID() == ENGINE_OBJ_ID_SCN_SKYBOX)
	//							{
	//								//Tells the shader how many textures this gameobject supports
	//								GLint textureQuantity = glGetUniformLocation(shaderProgramID, "textureProcessInfo.textureCount");
	//								glUniform1i(textureQuantity, 1); //Skybox is always single texture (combination of 6 BMPs)

	//								//Tells the shader what's the first texture for this mesh
	//								int startTextureIndex = GetLoadedTextureIndex(textures[0]->path);
	//								GLint textureIndex = glGetUniformLocation(shaderProgramID, "textureIndex");
	//								glUniform1i(textureIndex, startTextureIndex);

	//								glCullFace(GL_FRONT_AND_BACK);

	//								int currentTexture = GetLoadedTextureIndex(entity->GetName());

	//								glActiveTexture(GL_TEXTURE28 + currentTexture);			// Texture Unit x, max is GL_TEXTURE31
	//								glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0]->GetID());	// Texture now assoc with texture unit 0

	//								GLint skyBoxSampler_UL = glGetUniformLocation(shaderProgramID, "textureProcessInfo.skybox");
	//								glUniform1i(skyBoxSampler_UL, 28 + currentTexture);	// Texture unit 26
	//							}
	//						}
	//					}

	//					//Enable shader features
	//					WriteShaderFeatures(entity, mesh, mc, shaderProgramID);
	//				}
	//				else
	//				{
	//					//Do not draw
	//				}
	//			}
	//		}
	//	}
	//}
}

void GameSystemVideo::SetGLCallbacks()
{
	glfwSetWindowSizeCallback
	(
		window,
		[](GLFWwindow* window, int width, int height)
		{
			systemVideo->SetWindowSize(width, height);
			//glViewport(0, 0, systemVideo->windowWidth, systemVideo->windowHeight);

			if (systemVideo->onWindowResizedEvent)
			{
				systemVideo->onWindowResizedEvent(width, height);
			}
		}
	);
}

const bool GameSystemVideo::DrawScene(FBO* destination)
{	
	bool rc = false;
	//glUseProgram(shaderProgramID);

	rc = BindFrameBuffer(destination);
	if (!rc)
	{
		//An error has occurred while binding the framebuffer.
		return false;
	}

	//GL functions
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearStencil(42);
	/*glEnable(GL_DEPTH);*/
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	glDisable(GL_STENCIL_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	if (scene)
	{
		if (!scene->GetLights())
		{
			SetLastErrorString("The scene has no lights!");
			return false;
		}
		//DrawLights(*scene->GetLights());

		///*If the scene is being drawn to a framebuffer, then use the camera specified by the buffer*/
		//ICamera* camera = nullptr;
		//if (destination && destination->GetType() == "FBO" && !destination->GetCameraReference().empty())
		//{
		//	/*Checks if there is a pointer to the camera in the global map of cameras.*/
		//	auto camIT = cameraReferenceMap.find(destination->GetCameraReference());
		//	if (camIT != cameraReferenceMap.end())
		//	{
		//		camera = camIT->second;
		//	}
		//	/*Use the camera from the current scene if not.*/
		//	else
		//	{
		//		camera = scene->GetCamera();
		//	}
		//}
		//else
		//{
		//	camera = scene->GetCamera();
		//}

		//if (!camera)
		//{
		//	SetLastErrorString("The scene has no camera!");
		//	return false;
		//}
		//UpdateCamera(camera);
		
		//When the passID is a negative number, then we're drawing to an FBO object
		//If 0 (or higher) it draws to the default framebuffer or the actual screen
		
		IShaderProgram* program = this->shaderContainer->FindShaderProgramByIndex(0);

		//The current deltaTime
		GLint deltaTime_UL = glGetUniformLocation(program->GetID(), "timeInfo.deltaTime");
		glUniform1f(deltaTime_UL, this->frameTime.GetDeltaTime());

		//The current runtime
		GLint runTime_UL = glGetUniformLocation(program->GetID(), "timeInfo.runTime");
		glUniform1f(runTime_UL, (GLfloat)this->frameTime.GetRunTime());

		//Set the actual screen size
		GLint resolution_UL = glGetUniformLocation(program->GetID(), "screenResolution");
		glUniform2f(resolution_UL, (GLfloat)windowWidth, (GLfloat)windowHeight);

		std::vector<Entity*> entities;
		if (TransparencySort(entities))
		{
			for (auto& entity : entities)
			{
				if (entity->IsDisposing())
				{
					/*If disposing, ignore.*/
					continue;
				}

				IRender* renderComponent = entity->GetComponent<IRender*>();
				if (!renderComponent)
				{
					/*Entity is not render component*/
					continue;
				}

				if (!renderComponent->IsVisible())
				{
					/*Ignores invisible objects.*/
					continue;
				}

				if (systemFlags & (unsigned long long)SystemFlags_e::DonotDrawEntitiesNotFacingCamera)
				{
					//if (!camera->IsFacingEntity(entity) && entity->GetTypeID() != ENGINE_OBJ_ID_SCN_SKYBOX)
					//{
					//	/*Ignores objects that are not facing the camera.*/
					//	continue;
					//}
				}

				//Write pass id to shader
				GLint passArrayCount_UL = glGetUniformLocation(program->GetID(), "passProcessInfo.passArrayCount");

				/*Checks if this GameObject is the target for an FBO.
				  If it is, then draw this object with the FBO texture instead of a regular one.*/
				auto itPassID = std::find_if(graphicBufferReferenceMap.begin(), graphicBufferReferenceMap.end(), [entity](std::pair<std::string, IGraphicsBuffer*> p)
					{
						if (p.second->GetType() == "FBO")
						{
							return ((FBO*)p.second)->GetTarget() == entity->GetName();
						}
						else if (p.second->GetType() == "StencilBuffer")
						{
							return ((StencilBuffer*)p.second)->GetTarget() == entity->GetName();
						}
						else
						{
							return false;
						}
					});
				if (itPassID == graphicBufferReferenceMap.end())
				{
					/*Regular object, draw normally, with with pass number 0.*/
					glUniform1i(passArrayCount_UL, (GLint)0);
					DrawObject(entity);
				}
				else
				{
					IGraphicsBuffer* graphicsbuffer = (*itPassID).second;
					if (graphicsbuffer->GetType() == "FBO")
					{
						/*Draws the object as an FBO target.*/
						FBO* fbo = (FBO*)graphicsbuffer;

						std::vector<std::pair<int, bool>> passes;
						if (fbo->GetPasses(passes, true))
						{
							size_t passSz = passes.size();

							glUniform1i(passArrayCount_UL, (GLint)passSz);

							char tempPassArrayStr[255];
							for (size_t i = 0; i < passSz; i++)
							{
								//Write pass id to shader
								sprintf_s(tempPassArrayStr, 255, "passProcessInfo.passArray[%llu]", i);

								GLint passArrayItem_UL = glGetUniformLocation(program->GetID(), tempPassArrayStr);
								glUniform1i(passArrayItem_UL, (GLint)passes[i].first);
							}

							DrawFBO(entity, fbo);
						}
					}
					else if (graphicsbuffer->GetType() == "StencilBuffer")
					{
						/////*Regular object, draw normally, with with pass number 0.*/
						////glUniform1i(passNumber, (GLint)0);
						////DrawObject(go, shaderProgramID);

						StencilBuffer* stencil = (StencilBuffer*)graphicsbuffer;
						std::vector<int> passIDs = stencil->GetPassIDs();

						glUniform1i(passArrayCount_UL, (GLint)passIDs.size());

						char tempPassArrayStr[255];
						for (size_t i = 0; i < passIDs.size(); i++)
						{
							//Write pass id to shader
							sprintf_s(tempPassArrayStr, 255, "passProcessInfo.passArray[%llu]", i);

							GLint passArrayItem_UL = glGetUniformLocation(program->GetID(), tempPassArrayStr);
							glUniform1i(passArrayItem_UL, (GLint)passIDs[i]);
						}

						DrawStencil(entity, stencil);
					}
					else
					{
						//Not supported
					}
				}

				if (this->isDebugRenderingActive)
				{
					RenderCollider(entity);
				}
			}
		}
		else
		{
			SetLastErrorString("Could not sort objects for transparency.");
			return false;
		}
	}
	else
	{
		SetLastErrorString("The scene is nullptr.");
		return false;
	}	

	return true;	
}

void GameSystemVideo::Dispose()
{
	DisposePointer(this->shaderContainer);

	DisposeVector(glShaders);

	DisposePointer(textureManager);

	DisposePointer(debugRenderer);

	//Clear textures -- texture pointers are cleared by the meshes
	loadedTextures.clear();

	//Clear VAOs	
	VAOMap.clear();

	DisposeShaderPrograms();

	//Do not delete the framebuffers
	//FBOs are deleted by the levels
	//This just removes the FBO pointers from the opengl framework.
	//ClearFrameBuffers();
}

void GameSystemVideo::Update(const FrameTime& frameTime)
{
	if (!this->isActive.load()) return;
	this->isUpdateComplete.store(false);

	SetFrameTime(frameTime);

	/*Checks if the scene is supposed to be drawn. Scenes can also serve as containers for things that will be used somewhere else.*/
	if (scene->ShouldDraw())
	{
		/*If the scene is using an offscreen buffer, draw the same scene to all buffers.*/
		if (scene->IsUsingCustomFBO())
		{
			/*Draws a scene to an FBO.*/
			for (std::string fboName : *scene->GetFrameBufferNames())
			{
				/*Checks if there is a buffer with that name*/
				auto it = std::find_if(graphicBufferReferenceMap.begin(), graphicBufferReferenceMap.end(), [fboName](std::pair<std::string, IGraphicsBuffer*> p)
					{
						return p.second->GetType() == "FBO" && ((FBO*)p.second)->GetName() == fboName;
					});
				if (it != graphicBufferReferenceMap.end())
				{
					DrawScene((FBO*)it->second);
					if(this-isDebugRenderingActive) DrawDebugObjects();
				}
			}

			/*Check if the scene should also be drawn to main buffer*/
			if (scene->DrawToMainBuffer())
			{
				DrawScene();
				if (this - isDebugRenderingActive) DrawDebugObjects();
			}
		}
		/*Draws a scene that is not attached to an FBO.*/
		else
		{
			/*If the scene is not using a custom buffer, draw to main buffer*/
			DrawScene();
			if (this - isDebugRenderingActive) DrawDebugObjects();
		}

		//Bind to default buffer again
		bool rc = BindFrameBuffer();
	}

	/*glUseProgram(currentShaderProgram->id);
	glfwSwapBuffers(window);*/

	this->isUpdateComplete.store(true);
}
