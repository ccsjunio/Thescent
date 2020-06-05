#include "GameSystemGUI.h"
#include <EngineFolders.h>
#include "GameSystemUserControl.h"
#include <ILight.h>
#include <LightPoint.h>
#include <LightSpot.h>
#include <LightDirectional.h>
#include <IRigidbody.h>
#include <imgui\imGuIZMOquat.h>
#include <IRender.h>
#include <IMeshComponent.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>
#include <GameWorld.h>
#include "GameWindowFlags.h"
#include <Texture2D.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
constexpr auto TEXTURE_NAME_LOGO = "THESCENT_LOGO";
constexpr auto TEXTURE_NAME_LASERBLASTER = "Laser\nBlaster\n[1]";
constexpr auto TEXTURE_NAME_WTWCANNON = "WTW\nCannon\n[2]";
constexpr auto TEXTURE_NAME_ANTICLOAK = "Anticloak\n[N]";

bool Combo(const char* label, int* current_item, const std::vector<std::string>& items, int height_in_items = -1)
{
	int items_count = items.size();
	return ImGui::Combo
	(
		label, 
		current_item, 
		[](void* data, int idx, const char** out_text) 
		{ 
			*out_text = ((const std::vector<std::string>*)data)->at(idx).c_str(); 
			return true; 
		}, 
		(void*)&items, items_count, height_in_items
	);
}

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

ITexture* GameSystemGUI::GetItemTexture(const unsigned int& id)
{
	std::string textureName;

	switch (id)
	{
	default:
	case 0:
	{
		return nullptr;
	}
	break;
	case 1:
	{
		textureName = TEXTURE_NAME_ANTICLOAK;
	}
	break;
	}

	return FindGUITextureByName(textureName);
}

ITexture* GameSystemGUI::GetWeaponTexture(const unsigned int& weaponID)
{
	std::string textureName;

	switch (weaponID)
	{
	default:
	case 0:
	{
		textureName = TEXTURE_NAME_LASERBLASTER;
	}
		break;
	case 1:
	{
		textureName = TEXTURE_NAME_WTWCANNON;
	}
	break;
	}

	return FindGUITextureByName(textureName);
}

void GameSystemGUI::LoadGUITextures()
{
	Texture2D* logoTexture = new Texture2D();
	logoTexture->SetName(TEXTURE_NAME_LOGO);
	logoTexture->SetPath(EngineFolders::GetTexturesPath() + "GUI\\logo.png");
	if (!textureManager->Create2DTexture(logoTexture, true))
	{
		printf("Could not load Logo texture!\n");
		DisposePointer(logoTexture);
	}
	guiTextures.push_back(logoTexture);

	Texture2D* laserTexture = new Texture2D();
	laserTexture->SetName(TEXTURE_NAME_LASERBLASTER);
	laserTexture->SetPath(EngineFolders::GetTexturesPath() + "GUI\\LaserBlaster.png");
	if (!textureManager->Create2DTexture(laserTexture, true))
	{
		printf("Could not load LaserBlaser texture!\n");
		DisposePointer(laserTexture);
	}
	guiTextures.push_back(laserTexture);

	Texture2D* cannonTexture = new Texture2D();
	cannonTexture->SetName(TEXTURE_NAME_WTWCANNON);
	cannonTexture->SetPath(EngineFolders::GetTexturesPath() + "GUI\\Cannon.png");
	if (!textureManager->Create2DTexture(cannonTexture, true))
	{
		printf("Could not load Cannon texture!\n");
		DisposePointer(cannonTexture);
	}
	guiTextures.push_back(cannonTexture);

	Texture2D* anticloakTexture = new Texture2D();
	anticloakTexture->SetName(TEXTURE_NAME_ANTICLOAK);
	anticloakTexture->SetPath(EngineFolders::GetTexturesPath() + "GUI\\anticloak.png");
	if (!textureManager->Create2DTexture(anticloakTexture, true))
	{
		printf("Could not load Anticloak texture!\n");
		DisposePointer(anticloakTexture);
	}
	guiTextures.push_back(anticloakTexture);
}

void GameSystemGUI::RenderGameObjectControls(Entity* go)
{
	IRender* vr = go->GetComponent<IRender*>();
	if (vr)
	{
		if (ImGui::TreeNode("Visibility"))
		{
			static bool goIsVisible = true;
			if(ImGui::Checkbox("Is Visible", &goIsVisible))
			{
				vr->SetVisibility(goIsVisible);
			}

			static int display_mode = 0;
			if (ImGui::Combo("Display Mode", &display_mode, "Fill\0Wireframe\0"))
			{
				switch (display_mode)
				{
				case 0:
					vr->SetRenderMode(GL_FILL);
					break;
				case 1:
					vr->SetRenderMode(GL_LINE);
					break;
				default:
					break;
				}
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Color Properties"))
		{
			if (ImGui::TreeNode("Diffuse Color"))
			{
				Vector4D color = vr->GetDiffuse();
				if (AddColorControl(color))
				{
					vr->SetDiffuse(color);
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Specular Color"))
			{
				Vector4D color = vr->GetSpecular();
				if (AddColorControl(color))
				{
					vr->SetSpecular(color);
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}

	IRigidbody* rb = go->GetComponent<IRigidbody*>();
	if (rb)
	{
		if (ImGui::TreeNode("Rotation"))
		{
			Quaternion qRot = rb->GetRotation();
			if (ImGui::gizmo3D("##goRotation", qRot /*, size,  mode */))
			{
				rb->SetRotation(qRot);
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Scale"))
		{
			Vector3D currScale = rb->GetScale();

			char buff[255];
			sprintf_s(buff, 255, "Scaling of rigidbody %s", rb->GetName().c_str());
			if (ImGui::InputFloat(buff, &currScale.x, 0.25f, 0.25f, "%.5f"))
			{
				currScale = Vector3D(currScale.x, currScale.x, currScale.x);
				rb->SetScale(currScale);
			}

			ImGui::TreePop();
		}
	}
	
	if (ImGui::Button("Clone"))
	{
		auto newGo = go->Clone();

		GameSystemUserControl* uc = ((GameSystemUserControl*)systemUserControl);
		uc->AddNewObject(newGo);
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		GameSystemUserControl* uc = ((GameSystemUserControl*)systemUserControl);		
		uc->RemoveObject(go);
	}
}

void GameSystemGUI::RenderLightControls(ILight* light)
{
	if (ImGui::TreeNode("Visibility"))
	{
		static bool lightIsVisible = true;
		ImGui::Checkbox("Is Visible", &lightIsVisible);
		{
			light->SetVisibility(lightIsVisible);
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Diffuse Color"))
	{
		Vector4D diffuse = light->GetDiffuse();
		if (AddColorControl(diffuse))
		{
			light->SetDiffuse(diffuse);
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Specular Color"))
	{
		Vector4D specular = light->GetSpecular();
		if (AddColorControl(specular))
		{
			light->SetSpecular(specular);
		}
		ImGui::TreePop();
	}

	std::string lightType = light->GetType();
	if (lightType == "LightPoint")
	{
		LightPoint* l = (LightPoint*)light;

		if (ImGui::TreeNode("Attenuation"))
		{
			float constatten = l->GetConstantAttenuation();
			if (ImGui::InputFloat("Constant", &constatten, 0.00001f, 0.00001f, "%.5f"))
			{
				l->SetConstantAttenuation(constatten);
			}

			float linearatten = l->GetLinearAttenuation();
			if (ImGui::InputFloat("Linear", &linearatten, 0.00001f, 0.00001f, "%.5f"))
			{
				l->SetLinearAttenuation(linearatten);
			}

			float quadatten = l->GetQuadraticAttenuation();
			if (ImGui::InputFloat("Quadratic", &quadatten, 0.00001f, 0.00001f, "%.5f"))
			{
				l->SetQuadraticAttenuation(quadatten);
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Cutoff Distance"))
		{
			float cutoffdist = l->GetCutoffDistance();
			if (ImGui::InputFloat("Cutoff Distance", &cutoffdist, 1.0f, 1.0f, "%.5f"))
			{
				l->SetCutoffDistance(cutoffdist);
			}

			ImGui::TreePop();
		}
	}
	else if (lightType == "LightSpot")
	{
		LightSpot* l = (LightSpot*)light;

		if (ImGui::TreeNode("Direction"))
		{
			Vector3D dir = l->GetDirection();
			if (ImGui::gizmo3D("##lightDir", dir /*, size,  mode */))
			{
				l->SetDirection(dir);
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Angles"))
		{
			float innerAngle = glm::radians(l->GetInnerAngle());
			if (ImGui::SliderAngle("Inner Angle", &innerAngle, 0.0f, 360.0f))
			{
				l->SetInnerAngle(glm::degrees(innerAngle));
			}
			float outerAngle = glm::radians(l->GetOuterAngle());
			if (ImGui::SliderAngle("Outer Angle", &outerAngle, 0.0f, 360.0f))
			{
				l->SetOuterAngle(glm::degrees(outerAngle));
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Attenuation"))
		{
			float constatten = l->GetConstantAttenuation();
			if (ImGui::InputFloat("Constant", &constatten, 0.00001f, 0.00001f, "%.5f"))
			{
				l->SetConstantAttenuation(constatten);
			}

			float linearatten = l->GetLinearAttenuation();
			if (ImGui::InputFloat("Linear", &linearatten, 0.00001f, 0.00001f, "%.5f"))
			{
				l->SetLinearAttenuation(linearatten);
			}

			float quadatten = l->GetQuadraticAttenuation();
			if (ImGui::InputFloat("Quadratic", &quadatten, 0.00001f, 0.00001f, "%.5f"))
			{
				l->SetQuadraticAttenuation(quadatten);
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Cutoff Distance"))
		{
			float cutoffdist = l->GetCutoffDistance();
			if (ImGui::InputFloat("Cutoff Distance", &cutoffdist, 1.0f, 1.0f, "%.5f"))
			{
				l->SetCutoffDistance(cutoffdist);
			}

			ImGui::TreePop();
		}
	}
	else if (lightType == "LightDirectional")
	{
		LightDirectional* l = (LightDirectional*)light;

		if (ImGui::TreeNode("Direction"))
		{
			Vector3D dir = l->GetDirection();
			if (ImGui::gizmo3D("##lightDir", dir /*, size,  mode */))
			{
				l->SetDirection(dir);
			}

			ImGui::TreePop();
		}
	}
	else
	{
		//Unknown light
	}

	if (ImGui::Button("Clone"))
	{
		ILight* newGo = (ILight*)light->Clone();

		GameSystemUserControl* uc = ((GameSystemUserControl*)systemUserControl);
		//uc->AddNewObject(newGo);
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		GameSystemUserControl* uc = ((GameSystemUserControl*)systemUserControl);
		//uc->RemoveObject(light);
	}
}

const bool GameSystemGUI::AddColorControl(Vector4D& color)
{
	bool rc = false;

	//ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	//ImGui::Text("Color picker:");
	static bool alpha = true;
	static bool alpha_bar = true;
	static bool side_preview = true;
	static bool ref_color = false;
	static ImVec4 ref_color_v(1.0f, 0.0f, 1.0f, 0.5f);
	static int display_mode = 0;
	static int picker_mode = 0;
	ImGui::Checkbox("With Alpha", &alpha);
	ImGui::Checkbox("With Alpha Bar", &alpha_bar);
	ImGui::Checkbox("With Side Preview", &side_preview);
	if (side_preview)
	{
		ImGui::SameLine();
		ImGui::Checkbox("With Ref Color", &ref_color);
		if (ref_color)
		{
			ImGui::SameLine();
			ImGui::ColorEdit4("##RefColor", &ref_color_v.x, ImGuiColorEditFlags_NoInputs);// | misc_flags);
		}
	}
	ImGui::Combo("Display Mode", &display_mode, "Auto/Current\0None\0RGB Only\0HSV Only\0Hex Only\0");
	ImGui::SameLine(); HelpMarker("ColorEdit defaults to displaying RGB inputs if you don't specify a display mode, but the user can change it with a right-click.\n\nColorPicker defaults to displaying RGB+HSV+Hex if you don't specify a display mode.\n\nYou can change the defaults using SetColorEditOptions().");
	ImGui::Combo("Picker Mode", &picker_mode, "Auto/Current\0Hue bar + SV rect\0Hue wheel + SV triangle\0");
	ImGui::SameLine(); HelpMarker("User can right-click the picker to change mode.");
	ImGuiColorEditFlags flags;// = misc_flags;
	if (!alpha)            flags |= ImGuiColorEditFlags_NoAlpha;        // This is by default if you call ColorPicker3() instead of ColorPicker4()
	if (alpha_bar)         flags |= ImGuiColorEditFlags_AlphaBar;
	if (!side_preview)     flags |= ImGuiColorEditFlags_NoSidePreview;
	if (picker_mode == 1)  flags |= ImGuiColorEditFlags_PickerHueBar;
	if (picker_mode == 2)  flags |= ImGuiColorEditFlags_PickerHueWheel;
	if (display_mode == 1) flags |= ImGuiColorEditFlags_NoInputs;       // Disable all RGB/HSV/Hex displays
	if (display_mode == 2) flags |= ImGuiColorEditFlags_DisplayRGB;     // Override display mode
	if (display_mode == 3) flags |= ImGuiColorEditFlags_DisplayHSV;
	if (display_mode == 4) flags |= ImGuiColorEditFlags_DisplayHex;

	ImVec4 ig_color = ImVec4(color.r, color.g, color.b, color.a);
	if (ImGui::ColorPicker4("MyColor##4", (float*)&ig_color, flags, ref_color ? &ref_color_v.x : NULL))
	{
		color.r = ig_color.x;
		color.g = ig_color.y;
		color.b = ig_color.z;
		color.a = ig_color.w;
		rc = true;
	}

	ImGui::Text("Programmatically set defaults:");
	ImGui::SameLine(); HelpMarker("SetColorEditOptions() is designed to allow you to set boot-time default.\nWe don't have Push/Pop functions because you can force options on a per-widget basis if needed, and the user can change non-forced ones with the options menu.\nWe don't have a getter to avoid encouraging you to persistently save values that aren't forward-compatible.");
	if (ImGui::Button("Default: Uint8 + HSV + Hue Bar"))
		ImGui::SetColorEditOptions(ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_PickerHueBar);
	if (ImGui::Button("Default: Float + HDR + Hue Wheel"))
		ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel);

	// HSV encoded support (to avoid RGB<>HSV round trips and singularities when S==0 or V==0)
	static ImVec4 color_stored_as_hsv(0.23f, 1.0f, 1.0f, 1.0f);
	ImGui::Spacing();
	ImGui::Text("HSV encoded colors");
	ImGui::SameLine(); HelpMarker("By default, colors are given to ColorEdit and ColorPicker in RGB, but ImGuiColorEditFlags_InputHSV allows you to store colors as HSV and pass them to ColorEdit and ColorPicker as HSV. This comes with the added benefit that you can manipulate hue values with the picker even when saturation or value are zero.");
	ImGui::Text("Color widget with InputHSV:");
	ImGui::ColorEdit4("HSV shown as RGB##1", (float*)&color_stored_as_hsv, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_Float);
	ImGui::ColorEdit4("HSV shown as HSV##1", (float*)&color_stored_as_hsv, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_Float);
	ImGui::DragFloat4("Raw HSV values", (float*)&color_stored_as_hsv, 0.01f, 0.0f, 1.0f);

	return rc;
}

void GameSystemGUI::RenderDeveloperWindow()
{
	ImGui::StyleColorsDark();
	ImGui::SetNextWindowPos(ImVec2(20, 20));
	ImGui::SetNextWindowSize(ImVec2(400, 150));
	ImGui::Begin("Developer Window", &show_developer_window);

	if (ImGui::Button("Level Editor"))
	{
		showLevelEditor = true;
	}

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	RenderExternalMessage();

	ImGui::End();
}

void GameSystemGUI::RenderOptionsWindow()
{
	ImGui::StyleColorsDark();
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(500, 300));

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;
	ImGui::Begin("Options", &show_options_menu, window_flags);

	if (ImGui::Checkbox("Toggle Physics Colliders (only for BellEngine physics)", &toggleColliders))
	{
		if (guiEventCallback)
		{
			ISystemGUI::GUIEvent_s gui_event;
			if (toggleColliders)
			{
				gui_event.eventName = "toggle_colliders_on";
			}
			else
			{
				gui_event.eventName = "toggle_colliders_off";
			}

			guiEventCallback(gui_event);
		}
	}

	if (ImGui::Checkbox("Show developer window", &show_developer_window))
	{
		
	}

	ImGui::End();
}

void GameSystemGUI::RenderMainMenu()
{
	ImGui::StyleColorsDark();
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(750, 600));
	bool hopen = true;
	ImGui::Begin("HUD", &hopen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs);

	ITexture* thescentLogo = FindGUITextureByName(TEXTURE_NAME_LOGO);
	if (thescentLogo)
	{
		unsigned int glID = 0;
		if (textureManager->FindTextureIDByString(thescentLogo->path, glID))
		{
			ImGui::Image((void*)(intptr_t)glID, ImVec2(720.0f, 80.0f));
		}
	}	

	//Options
	{
		ImGui::StyleColorsDark();
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));
		ImGui::Begin("Buttons", &hopen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		if (!isLoadingLevel.load())
		{
			ImVec2 buttonSz = ImVec2(100, 20);

			if (ImGui::Button("PLAY", buttonSz))
			{
				if (guiEventCallback)
				{
					GUIEvent_s gui_event;
					gui_event.eventName = "toggle_input_capture";

					guiEventCallback(gui_event);
				}
			}

			if (ImGui::Button("OPTIONS", buttonSz))
			{
				this->show_options_menu = true;
			}

			if (ImGui::Button("CONTROLS", buttonSz))
			{
				this->show_controls_window = true;
			}

			if (ImGui::Button("CREDITS", buttonSz))
			{
				this->show_credits_window = true;
			}

			if (ImGui::Button("EXIT", buttonSz))
			{
				if (guiEventCallback)
				{
					GUIEvent_s gui_event;
					gui_event.eventName = "exit_game";

					guiEventCallback(gui_event);
				}
			}
		}
		else
		{
			ImGui::Text("Loading Level...");
		}

		ImGui::End();
	}
	

	ImGui::End();
}

void GameSystemGUI::RenderLevelEditor()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;
	if (ImGui::Begin("Level Editor", &showLevelEditor, window_flags))
	{
		GameSystemUserControl* uc = ((GameSystemUserControl*)systemUserControl);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save Level To XML"))
				{
					ISystemGUI::GUIEvent_s gui_event;
					gui_event.eventName = "save_state";
					guiEventCallback(gui_event);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		if (uc->selectedItem)
		{
			IIdentifiable* item = dynamic_cast<IIdentifiable*>(uc->selectedItem);
			if (item)
			{
				if (ImGui::TreeNode("Identification"))
				{
					if (item)
					{
						ImGui::Text("Type: %s", item->GetType().c_str());

						ImGui::Text("Name: ");
						ImGui::SameLine();
						static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
						char text[255]; sprintf_s(text, 255, "%s", item->GetName().c_str());
						if (ImGui::InputTextMultiline("##name", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 1.5f), flags))
						{
							item->SetName(text);
						}
					}

					ImGui::TreePop();
				}
			}

			Entity* go = dynamic_cast<Entity*>(uc->selectedItem);
			if (go)
			{
				//Is a gameobject, show controls for gameobject
				RenderGameObjectControls(go);
			}
			//If not a gameobject, then test for something else
			else
			{
				ILight* light = dynamic_cast<ILight*>(uc->selectedItem);
				if (light)
				{
					//Is light, show controls for light
					RenderLightControls(light);
				}
				//Test for something else
				else
				{

				}
			}
		}
		else
		{
			//No item is selected
		}

		ImGui::End();
	}
}

GameSystemGUI::GameSystemGUI()
{
	this->hideThescentPanel.store(false);
	this->isLoadingLevel.store(true);
	this->selectedWeapon.store(0u);
	this->selectedItem.store(0u);
	this->screenProperties = ISystemGUI::ScreenProperties_s();
	this->screenProperties.screenHeight = 768u;
	this->screenProperties.screenWidth = 1024u;
	this->textureManager = nullptr;
	this->isUpdateComplete = false;
	this->isActive = true;
	this->world = nullptr;
	this->window = nullptr;
	this->systemUserControl = nullptr;
	this->iniPath = nullptr;
	this->show_demo_window = false;
	this->showLevelEditor = false;
	this->show_developer_window = false;
	this->show_main_menu = false;
	this->show_options_menu = false;
	this->show_anticloak_hint = false;
	this->show_credits_window = false;
	this->show_controls_window = false;
}

GameSystemGUI::~GameSystemGUI()
{
	Dispose();
}

void GameSystemGUI::SetScreenProperties(const ISystemGUI::ScreenProperties_s& properties)
{
	this->screenProperties = properties;
}

void GameSystemGUI::ShowMainMenu(const bool& value)
{
	this->show_main_menu = value;
}

const bool GameSystemGUI::FindMessageByContextFlags(const std::vector<unsigned long long>& flags, std::vector<GUIMessage_s*>& messages)
{
	for (auto& flag : flags)
	{
		for (ISystemGUI::GUIMessage_s*& msg : this->messages)
		{
			if (msg->contextFlags & flag)
			{
				messages.push_back(msg);
			}
		}
	}

	return !messages.empty();
}

const bool GameSystemGUI::PushMessage(const unsigned int& flags, const std::string& text, const double& duration, const unsigned int& renderAs)
{
	/*Only add messages that are different from one another.*/
	auto it = std::find_if(messages.begin(), messages.end(), [text](GUIMessage_s* msg) { return msg->text == text; });
	if (it == messages.end())
	{
		GUIMessage_s* msg = new GUIMessage_s();
		msg->contextFlags = flags;
		msg->text = text;
		msg->duration = duration;
		msg->timeacc = 0.0;
		msg->renderAs = renderAs;

		messages.push_back(msg);

		return true;
	}

	return false;
}

const bool GameSystemGUI::PopMessage(ISystemGUI::GUIMessage_s* message)
{
	auto it = std::find(messages.begin(), messages.end(), message);
	if (it != messages.end())
	{
		messages.erase(it);

		DisposePointer(message);
	}

	return false;
}

const bool GameSystemGUI::IsUpdateComplete()
{
	return this->isUpdateComplete;
}

const bool GameSystemGUI::Initialize(ParameterContainer& input)
{
	Parameter p;
	bool rc = input.FindParameterByName("window", p);
	if (!rc) return false;

	this->window = (GLFWwindow*)p.valuePtr;
	if (!this->window) return false;

	rc = input.FindParameterByName("world", p);
	if (!rc) return false;

	this->world = (GameWorld*)p.valuePtr;
	if (!this->world) return false;

	this->systemUserControl = (ISystemUserControl*)world->GetSystem("usercontrol");
	if (!this->systemUserControl) return false;

	auto videoSystem = (SystemVideo*)world->GetSystem("video");
	if (!videoSystem) return false;
	this->textureManager = videoSystem->GetTextureManager();
	LoadGUITextures();

	ImGui::CreateContext();

	auto iniPathStr = EngineFolders::GetConfigPath() + "gui.ini";
	//iniPath = iniPathStr.c_str();
	////sprintf_s(iniPath, 255, "%s", iniPathStr.c_str());
	//ImGui::GetIO().IniFilename = iniPath;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	if (!ImGui_ImplGlfw_InitForOpenGL(this->window, true))
	{
		SetLastErrorString("ImGui_ImplGlfw_InitForOpenGL failed!");
		return false;
	}

	if (!ImGui_ImplOpenGL3_Init("#version 430 core"))
	{
		SetLastErrorString("ImGui_ImplOpenGL3_Init failed!");
		return false;
	}

	return true;
}

void GameSystemGUI::BindGUICallback(std::function<void(const GUIEvent_s&)> fn)
{
	if (!fn) return;

	guiEventCallback = fn;
}

const bool GameSystemGUI::IsActive()
{
	return isActive;
}

void GameSystemGUI::Activate(const bool& value)
{
	this->isActive = value;
}

void GameSystemGUI::Render()
{
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//Shows ImGui's demo window
	/*if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}*/
		
	if (!this->isLoadingLevel.load() && !this->hideThescentPanel.load())
	{
		RenderHealthWindow();
		RenderLeftWeaponPanel();

		if (this->selectedItem.load() != 0)
		{
			RenderRightItemPanel();
		}
	}

	if (this->show_anticloak_hint)
	{
		RenderAnticloakHint();
	}

	if (show_main_menu)
	{
		RenderMainMenu();

		if (show_options_menu)
		{
			RenderOptionsWindow();
		}
	}

	if (show_controls_window)
	{
		RenderControls();
	}

	if (show_credits_window)
	{
		RenderCredits();
	}

	if (show_developer_window)
	{
		RenderDeveloperWindow();
	}

	if (showLevelEditor)
	{
		RenderLevelEditor();
	}

	// Rendering
	ImGui::Render();

	//Review this later
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GameSystemGUI::RenderExternalMessage()
{
	if (messages.empty()) return;

	std::vector<GUIMessage_s*> messagesToDelete;

	for (GUIMessage_s* msg : messages)
	{
		/*If the message is of infinite duration, render.*/
		if (msg->duration == 0.0)
		{
			GameSystemGUI::RenderContainer_e container = (GameSystemGUI::RenderContainer_e)msg->renderAs;
			switch (container)
			{
			case GameSystemGUI::RenderContainer_e::Text:
				ImGui::Text(msg->text.c_str());
				break;
			case GameSystemGUI::RenderContainer_e::TextBox:
				break;
			default:
				return;
			}
		}
		/*Otherwise...*/
		else
		{
			/*If the time accumulator is smaller than the duration, render.*/
			if (msg->timeacc < msg->duration)
			{
				GameSystemGUI::RenderContainer_e container = (GameSystemGUI::RenderContainer_e)msg->renderAs;
				switch (container)
				{
				case GameSystemGUI::RenderContainer_e::Text:
					ImGui::Text(msg->text.c_str());
					break;
				case GameSystemGUI::RenderContainer_e::TextBox:
					break;
				default:
					return;
				}
				
				msg->timeacc += frameTime.GetDeltaTime();
			}
			/*Else, the timespan is complete, remove message from stack.*/
			else
			{
				messagesToDelete.push_back(msg);
			}
		}		
	}

	/*Removes messages from stack.*/
	for (GUIMessage_s* msg : messagesToDelete)
	{
		auto it = std::find(messages.begin(), messages.end(), msg);
		if (it != messages.end())
		{
			messages.erase(it);
			DisposePointer(msg);
		}
	}
}

void GameSystemGUI::RenderHealthWindow()
{
	ImGui::StyleColorsDark();

	ImGui::SetNextWindowPos(ImVec2(498, 573));
	ImGui::SetNextWindowSize(ImVec2(50, 20));

	bool hopen = true;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));
	ImGui::Begin("Health", &hopen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs);	
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::SetWindowFontScale(1.5);

	std::vector<unsigned long long> flags; 
	flags.push_back(GUIMSG_HEALTH);

	std::vector<ISystemGUI::GUIMessage_s*> msgs;
	if (FindMessageByContextFlags(flags, msgs))
	{
		for (auto& msg : msgs)
		{
			ImVec4 textColor = ImVec4();
			float value = strtof(msg->text.c_str(), NULL);
			if (value <= 30.0f)
			{
				textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			}
			else if (value > 30.f && value <= 80.f)
			{
				textColor = ImVec4(199 / 255.0f, 142 / 255.0f, 43 / 255.0f, 1.00);
			}
			else
			{
				textColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
			}

			ImGui::TextColored(textColor, msg->text.c_str());
			PopMessage(msg);
		}
	}

	ImGui::End();
}

void GameSystemGUI::RenderLeftWeaponPanel()
{
	ITexture* texture = GetWeaponTexture(selectedWeapon.load());
	if (texture)
	{
		unsigned int glID = 0;
		if (textureManager->FindTextureIDByString(texture->path, glID))
		{	
			ImGui::StyleColorsDark();

			//Weapon Texture
			{
				ImGui::SetNextWindowPos(ImVec2(190, 640));
				ImGui::SetNextWindowSize(ImVec2(175, 120));
				bool hopen = true;

				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));
				ImGui::Begin("Left Weapon Panel PIC", &hopen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs);
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::Image((void*)(intptr_t)glID, ImVec2(120, 120));

				ImGui::End();
			}
			
			//Weapon Info
			{
				ImGui::SetNextWindowPos(ImVec2(250, 640));
				ImGui::SetNextWindowSize(ImVec2(100, 100));
				bool hopen = true;

				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));
				ImGui::Begin("Left Weapon Panel NFO", &hopen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs);
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::SetWindowFontScale(1.5f);

				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.00), texture->GetName().c_str());

				ImGui::End();
			}
		}
	}
}

void GameSystemGUI::RenderRightItemPanel()
{
	ITexture* texture = GetItemTexture(selectedItem.load());
	if (texture)
	{
		unsigned int glID = 0;
		if (textureManager->FindTextureIDByString(texture->path, glID))
		{
			ImGui::StyleColorsDark();

			//Weapon Texture
			{
				ImGui::SetNextWindowPos(ImVec2(740, 650));
				ImGui::SetNextWindowSize(ImVec2(175, 120));
				bool hopen = true;

				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));
				ImGui::Begin("Right Item Panel PIC", &hopen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs);
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::Image((void*)(intptr_t)glID, ImVec2(100, 100));

				ImGui::End();
			}

			//Weapon Info
			{
				ImGui::SetNextWindowPos(ImVec2(650, 640));
				ImGui::SetNextWindowSize(ImVec2(120, 100));
				bool hopen = true;

				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));
				ImGui::Begin("Right Item Panel NFO", &hopen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs);
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::SetWindowFontScale(1.5f);

				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.00), texture->GetName().c_str());

				ImGui::End();
			}
		}
	}
}

void GameSystemGUI::RenderAnticloakHint()
{
	ImGui::StyleColorsDark();
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(300, 100));

	ImGui::Begin("Hint", &show_options_menu, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Press `N` to view invisible entities!");
	ImGui::Text("We`re counting on you captain!");

	//Button centered
	{
		ImGui::StyleColorsDark();
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.52f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		bool hopen = true;

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));
		ImGui::Begin("Buttons", &hopen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImVec2 buttonSz = ImVec2(100, 20);

		if (ImGui::Button("OK"))
		{
			ISystemGUI::GUIEvent_s gui_event;
			gui_event.eventName = "disable_night_vision_hint";
			guiEventCallback(gui_event);
		}

		ImGui::End();
	}

	ImGui::End();
}

void GameSystemGUI::RenderCredits()
{
	ImGui::StyleColorsDark();
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	//ImGui::SetNextWindowSize(ImVec2(600, 600));

	ImGui::Begin("Credits", &show_credits_window, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("\n");
	ImGui::Text("THESCENT is a final project for the Game Development: Advanced Programming Post-Graduate at Fanshawe College.");
	ImGui::Text("\n");
	ImGui::Text("THESCENT is inspired by the works of Interplay Entertainment on Descent(TM) (1995).");
	ImGui::Text("No copyright infrigement is intended with this software.");
	ImGui::Text("This project is under the MIT license.");
	ImGui::Text("\n");
	ImGui::Text("The author is Felipe Da Silva Bellini (felipe.bellini@outlook.com)");
	ImGui::Text("\n");
	ImGui::Text("To the teaching staff of the GDP, School Of IT 2019-2020:");
	ImGui::Text("*Feeney, Michael   - Animation, Graphics I & II, Physics I");
	ImGui::Text("*Gustafson, Lukas  - Configuration & Deployment, Network Programming");
	ImGui::Text("*Kelly, James      - Artificial Intelligence");
	ImGui::Text("*Lara, Oscar       - Media Fundamentals, Component Integration");
	ImGui::Text("*Lucas, James      - Physics II");
	ImGui::Text("*Maclam, Daniel    - Algorithms and Gems");
	ImGui::Text("\n");
	ImGui::Text("\n");
	ImGui::Text("Thanks for playing!");

	ImGui::End();
}

void GameSystemGUI::RenderControls()
{
	ImGui::StyleColorsDark();
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Default Controls", &show_controls_window, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("\n");
	ImGui::Text("General controls:");
	ImGui::Text("\n");
	ImGui::Text("[ESC] - Exit simulation");
	ImGui::Text("[P] - Pause game and show main menu");
	ImGui::Text("\n");
	ImGui::Text("\n");
	ImGui::Text("Player controls:");
	ImGui::Text("[W] - Move forward");
	ImGui::Text("[A] - Strafe left");
	ImGui::Text("[S] - Move backward");
	ImGui::Text("[D] - Strafe right");
	ImGui::Text("[Q] - Move up");
	ImGui::Text("[E] - Move down");
	ImGui::Text("[SPACEBAR] - Halt movement");
	ImGui::Text("[N] - Toggle anticloak vision");
	ImGui::Text("[1] - Select Laser Blaster");
	ImGui::Text("[2] - Select WTW Cannon");
	ImGui::Text("\n");
	ImGui::Text("\n");

	ImGui::End();
}

/*Processes specific messages in the system every frame*/
void GameSystemGUI::ReadMessages()
{
	std::vector<unsigned long long> flags;
	flags.push_back(GUIMSG_CHANGEWEAPON);
	flags.push_back(GUIMSG_LOADING);
	flags.push_back(GUIMSG_TOGGLEMENU);
	flags.push_back(GUIMSG_TOGGLENIGHTVISIONHINT);
	flags.push_back(GUIMSG_HIDESHIPPANEL);
	flags.push_back(GUIMSG_ITEMACQUIRED);

	std::vector<ISystemGUI::GUIMessage_s*> msgs;
	if (FindMessageByContextFlags(flags, msgs))
	{
		for (auto& msg : msgs)
		{
			if (msg->contextFlags & GUIMSG_CHANGEWEAPON)
			{
				/*Sets the weapon in use*/
				unsigned int id = strtoul(msg->text.c_str(), NULL, 0);
				selectedWeapon.store(id);
				/*Removes message from the stack*/
				PopMessage(msg);
			}
			else if (msg->contextFlags & GUIMSG_LOADING)
			{
				/*Sets the weapon in use*/
				bool state = strtoul(msg->text.c_str(), NULL, 0) != 0;
				isLoadingLevel.store(state);
				/*Removes message from the stack*/
				PopMessage(msg);
			}
			else if (msg->contextFlags & GUIMSG_TOGGLEMENU)
			{
				/*Sets the weapon in use*/
				bool state = strtoul(msg->text.c_str(), NULL, 0) != 0;
				ShowMainMenu(state);
				/*Removes message from the stack*/
				PopMessage(msg);
			}
			else if (msg->contextFlags & GUIMSG_TOGGLENIGHTVISIONHINT)
			{
				/*Sets the weapon in use*/
				bool state = strtoul(msg->text.c_str(), NULL, 0) != 0;
				show_anticloak_hint = state;
				/*Removes message from the stack*/
				PopMessage(msg);
			}
			else if (msg->contextFlags & GUIMSG_HIDESHIPPANEL)
			{
				/*Toggle visibility of the ship controls*/
				bool state = strtoul(msg->text.c_str(), NULL, 0) != 0;
				this->hideThescentPanel.store(state);
				/*Removes message from the stack*/
				PopMessage(msg);
			}
			else if (msg->contextFlags & GUIMSG_ITEMACQUIRED)
			{
				/*Selects an item*/
				unsigned int id = strtoul(msg->text.c_str(), NULL, 0);
				selectedItem.store(id);
				/*Removes message from the stack*/
				PopMessage(msg);
			}
		}
	}
}

ITexture* GameSystemGUI::FindGUITextureByName(const std::string& name)
{
	auto it = std::find_if(guiTextures.begin(), guiTextures.end(), [name](ITexture* tex) { return tex->GetName() == name; });
	if (it != guiTextures.end())
	{
		return *it;
	}

	return nullptr;
}

void GameSystemGUI::Dispose()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	DisposeVector(guiTextures);
}

void GameSystemGUI::HandleInput(const bool& option)
{
	if (option)
	{
		ImGui::GetIO().ConfigFlags = ImGuiConfigFlags_None;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		ImGui::CaptureKeyboardFromApp(true);
		ImGui::CaptureMouseFromApp(true);
	}
	else
	{
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		ImGui::CaptureKeyboardFromApp(false);
		ImGui::CaptureMouseFromApp(false);
	}
}

void GameSystemGUI::Update(const FrameTime& frameTime)
{
	if (!this->isActive) return;
	this->isUpdateComplete = false;

	SetFrameTime(frameTime);

	ReadMessages();

	Render();

	this->isUpdateComplete = true;
}
