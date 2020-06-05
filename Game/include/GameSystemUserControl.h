/***********************************************************************
* Project           : Game
* Author            : Felipe Bellini
* Date created      : Oct. 12 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <GLCommon.h>
#include <MathLibrary.h>
#include "GameConfig.h"
#include "Level.h"
#include "ILight.h"
#include <ISystemSound.h>
#include <ISystemUserControl.h>
#include <ISingleton.h>
#include <ISelectable.h>

/*For the animation project*/
#include "GameVertexAnimationSystem.h"
/**/

#define LAST_SELECTED_GAMEOBJECT		"GAMEOBJECT"
#define LAST_SELECTED_LIGHT				"LIGHT"
#define LAST_SELECTED_SOUND				"SOUND"
#define LAST_SELECTED_CHANNELGROUP		"CHANNELGROUP"
#define LAST_SELECTED_OBJECTGROUP		"OBJECTGROUP"

class GameSystemUserControl : public ISystemUserControl, public ISingleton
{
private:
	std::function<void(const KeyPress_s&)> keypressCallback;
	std::function<void(const MouseKey_s&)> mousepressCallback;
	std::function<void(const MouseMove_s&)> mousemoveCallback;

protected:
	GameSystemUserControl();

public:
	static GameSystemUserControl* systemUserControl;
	bool isActive, isUpdateComplete;
	GLFWwindow* window;
	ISystemSound* systemSound;
	ILight* selectedLight;	
	ISound* selectedSound;
	std::string lastSelection;
	Entity* selectedGameObject;
	ISelectable* selectedItem;
	GameConfig* gameConfig;
	Scene* selectedScene;
	float lastMouseXPosition;
	float lastMouseYPosition;
	bool captureInput = false;
	bool isWindowMaximized = false;
	bool isMouseLeftClick = false;
	bool isMouseRightClick = false;
	bool saveState = false;
	bool toggleColliders = false;
	bool moveNextLevel = false;
	bool movePreviousLevel = false;
	bool isShiftPressed = false;
	
	enum class ControllableTypes_e
	{
		Camera,
		GameObject,
		Light,
		ObjectGroup,
		Sound,
		Level,

		ControllableCount
	};
	std::string GetControllableTypesString(GameSystemUserControl::ControllableTypes_e ct)
	{
		switch (ct)
		{
		case GameSystemUserControl::ControllableTypes_e::Camera:
			return "Camera";
		case GameSystemUserControl::ControllableTypes_e::GameObject:
			return "GameObject";
		case GameSystemUserControl::ControllableTypes_e::Light:
			return "Light";
		case GameSystemUserControl::ControllableTypes_e::ObjectGroup:
			return "ObjectGroup";
		case GameSystemUserControl::ControllableTypes_e::Sound:
			return "Sound";
		case GameSystemUserControl::ControllableTypes_e::ControllableCount:
			return "ControllableCount";
		default:
			return "";
		}
	}
	GameSystemUserControl::ControllableTypes_e selectedControllableType;
		
	~GameSystemUserControl();
	
	const bool Initialize(ParameterContainer& input);
	void SetScene(Scene* scene);
	const bool IsCapturingInput();
	void ToggleInputCapture(const bool& value);
	void BindKeyboardPressCallback(std::function<void(const KeyPress_s&)> fn);
	void BindMouseClickCallback(std::function<void(const MouseKey_s&)> fn);
	void BindMouseMoveCallback(std::function<void(const MouseMove_s&)> fn);

	const bool IsUpdateComplete();
	static void* GetInstance();
	static void* GetInstance(unsigned int numberOfArguments, ...);
	void SetGLCallbacks();
	void AsyncProcessKeyPress();
	void AsyncProcessMousePress();
	void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);	
	void MouseMoveCallback(GLFWwindow* window, double xoffset, double yoffset);
	void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);
	void SaveState();	
	void ToggleWindowMode();
	void ToggleColliders();
	void NextLevel();
	void PreviousLevel();
	void SelectControllable(GameSystemUserControl::ControllableTypes_e selectedControllableType);	
	void SelectControllable();
	void MoveCamera();
	void MoveSound();
	void MoveLight();
	void Move();
	void MoveGameObject();
	void MoveSelectable();
	void ToggleInputCapture();
	void DisableSelectedItem();
	void SetSelectedItem(ISelectable* item);
	const bool IsActive();
	void Activate(const bool& value = true);
	void AddNewObject(ICloneable* obj);
	void RemoveObject(ICloneable* obj);
	Entity* FindEntityToCameraEye(const float& raycastLength);
	void Update(const FrameTime& deltaTime);

	//FOR THE ANIMATION MIDTERM
	bool moveCharacter = false; GameVertexAnimationSystem::Action_e movement_action;
	GameVertexAnimationSystem* systemVertexAnimation;
	void SetAnimationProject(GameVertexAnimationSystem* systemVertexAnimation)
	{
		this->systemVertexAnimation = systemVertexAnimation;
	}
	void ControlAnimatedCharacters();
};

