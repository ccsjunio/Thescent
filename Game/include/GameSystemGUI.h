#pragma once
#include <GLCommon.h>
#include <MathLibrary.h>
#include <ISystemGUI.h>
#include <Entity.h>
#include <ISystemUserControl.h>
#include <ILight.h>
#include <IWorld.h>
#include <IDisposable.h>
#include <TextureManager.h>
#include <atomic>

class GameSystemGUI : public ISystemGUI, public IDisposable
{
private:
	enum class RenderContainer_e
	{
		Text,
		TextBox
	};

	ISystemGUI::ScreenProperties_s screenProperties;
	std::vector<ITexture*> guiTextures;
	TextureManager* textureManager;
	GLFWwindow* window;
	IWorld* world;
	ISystemUserControl* systemUserControl;
	const char* iniPath;
	bool show_main_menu, show_options_menu, show_developer_window, show_anticloak_hint, show_credits_window, show_controls_window;
	bool show_demo_window = false;
	bool showLevelEditor = false;
	bool toggleColliders = false;
	bool isActive, isUpdateComplete;
	std::function<void(const ISystemGUI::GUIEvent_s&)> guiEventCallback;
	std::vector<GUIMessage_s*> messages;
	std::atomic<unsigned int> selectedWeapon, selectedItem;
	std::atomic<bool> isLoadingLevel, hideThescentPanel;

	ITexture* GetItemTexture(const unsigned int& weaponID);
	ITexture* GetWeaponTexture(const unsigned int& weaponID);
	void LoadGUITextures();
	void RenderGameObjectControls(Entity* go);
	void RenderLightControls(ILight* light);
	const bool AddColorControl(Vector4D& color);
	void RenderDeveloperWindow();
	void RenderOptionsWindow();
	void RenderMainMenu();
	void RenderLevelEditor();
	void Render();
	void RenderExternalMessage();
	void RenderHealthWindow();
	void RenderLeftWeaponPanel();
	void RenderRightItemPanel();
	void RenderAnticloakHint();
	void RenderCredits();
	void RenderControls();
	void ReadMessages();
	ITexture* FindGUITextureByName(const std::string& name);

public:
	GameSystemGUI();
	~GameSystemGUI();

	void SetScreenProperties(const ISystemGUI::ScreenProperties_s& properties);
	void ShowMainMenu(const bool& value);
	const bool FindMessageByContextFlags(const std::vector<unsigned long long>& flags, std::vector<GUIMessage_s*>& messages);
	const bool PushMessage(const unsigned int& flags, const std::string& text, const double& duration, const unsigned int& renderAs = 0u);
	const bool PopMessage(ISystemGUI::GUIMessage_s* message);
	const bool IsUpdateComplete();
	const bool Initialize(ParameterContainer& input);
	void BindGUICallback(std::function<void(const GUIEvent_s&)> fn);
	const bool IsActive();
	void Activate(const bool& value = true);
	void Dispose();
	void HandleInput(const bool& option);
	void Update(const FrameTime& deltaTime);
};

