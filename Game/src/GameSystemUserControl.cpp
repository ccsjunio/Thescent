/***********************************************************************
* Project           : Game
* Author            : Felipe Bellini
* Date created      : Oct. 12 - 2019
* Licensed under:	: MIT

************************************************************************/
#include "GameSystemUserControl.h"
#include <CameraLockedAt.h>
#include <CameraFree.h>
#include <LightPoint.h>
#include <LightSpot.h>
#include <LightDirectional.h>
#include <IMeshComponent.h>
#include <StringHelpers.h>
#include <BellEngineID.h>

GameSystemUserControl* GameSystemUserControl::systemUserControl = nullptr;

void GameSystemUserControl::SelectControllable()
{
	selectedControllableType = static_cast<ControllableTypes_e>
	(
		(static_cast<int>(selectedControllableType) + 1) % static_cast<int>(ControllableTypes_e::ControllableCount)
	);

	printf("Current controls are assigned to: %s\n", GetControllableTypesString(selectedControllableType).c_str());
}

void GameSystemUserControl::SelectControllable(GameSystemUserControl::ControllableTypes_e selectedControllableType)
{
	this->selectedControllableType = selectedControllableType;
	printf("Current controls are assigned to: %s\n", GetControllableTypesString(selectedControllableType).c_str());
}

void GameSystemUserControl::MoveGameObject()
{
	if (!selectedScene) return;

	if (selectedGameObject && isMouseLeftClick)
	{
		ICamera* camera = selectedScene->GetCamera();
		if (camera && camera->GetType() == "CameraLockedAt")
		{
			Entity* cameraTarget = ((CameraLockedAt*)camera)->GetTarget();
			if (cameraTarget)
			{
				auto fwd = cameraTarget->GetComponent<IRigidbody*>()->GetForwardInWorld();
				selectedGameObject->GetComponent<IRigidbody*>()->SetPosition(cameraTarget->GetComponent<IRigidbody*>()->GetWorldMatrix() * Vector4D(0.0f, 0.0f, fwd.z * 10.0f, 1.0f));
			}
		}
	}

	if (selectedItem)
	{
		Entity* go = dynamic_cast<Entity*>(selectedItem);
		if (go)
		{
			IRigidbody* rb = go->GetComponent<IRigidbody*>();
			if (rb)
			{
				Vector3D orientation = rb->GetOrientation();
				float moveForce = 1.0f;

				if (glfwGetKey(window, gameConfig->keyBindings->MoveLeft) == GLFW_PRESS)
				{
					rb->ApplyForce(Vector3D(orientation.z < 0.0f ? -moveForce : moveForce, 0.0f, 0.0f));
				}
				if (glfwGetKey(window, gameConfig->keyBindings->MoveRight) == GLFW_PRESS)
				{
					rb->ApplyForce(Vector3D(orientation.z < 0.0f ? moveForce : -moveForce, 0.0f, 0.0f));
				}
				if (glfwGetKey(window, gameConfig->keyBindings->MoveForward) == GLFW_PRESS)
				{
					rb->ApplyForce(Vector3D(0.0f, 0.0f, orientation.z < 0.0f ? -moveForce : moveForce));
				}
				if (glfwGetKey(window, gameConfig->keyBindings->MoveBackward) == GLFW_PRESS)
				{
					rb->ApplyForce(Vector3D(0.0f, 0.0f, orientation.z < 0.0f ? moveForce : -moveForce));
				}
			}
		}
	}
}

void GameSystemUserControl::MoveSelectable()
{
	if (!selectedScene) return;

	if (selectedItem /*&& isMouseLeftClick*/)
	{
		ICamera* camera = selectedScene->GetCamera();
		if (camera && camera->GetType() == "CameraLockedAt")
		{
			Vector3D camFWD = camera->GetForward();
			IRigidbody* cameraTarget = ((CameraLockedAt*)camera)->GetRigidbody();

			Entity* go = dynamic_cast<Entity*>(selectedItem);
			if (go)
			{
				IRigidbody* rb = go->GetComponent<IRigidbody*>();
				if (rb)
				{
					rb->SetPosition(cameraTarget->GetWorldMatrix() * Vector4D(rb->GetForwardInWorld() * 10.f, 1.0f));
				}

				return;
			}
					
			ILight* lt = dynamic_cast<ILight*>(selectedItem);
			if (lt)
			{
				lt->SetPosition(cameraTarget->GetWorldMatrix() * Vector4D(camFWD * 10.f, 1.0f));// Vector4D(0.0f, 0.0f, camFWD.z * 10.0f, 1.0f);
				return;
			}
		}
	}
}

void GameSystemUserControl::ToggleInputCapture()
{
	captureInput = !captureInput;
}

void GameSystemUserControl::DisableSelectedItem()
{
	if (selectedItem)
	{
		selectedItem->Deselect();
		/*For the physics project. When a gameobject is de-selected, change color of meshes to their original diffuse.*/
		Entity* go = dynamic_cast<Entity*>(selectedItem);
		if (go)
		{
			std::vector<IMeshComponent*> mCs;
			if (go->GetComponents(mCs))
			{
				for (IMeshComponent* mc : mCs)
				{
					std::vector<IMesh*> meshes;
					if (mc->GetMeshes(meshes))
					{
						for (IMesh* mesh : meshes)
						{
							mesh->SetDiffuse(mesh->GetBaseDiffuseColor());
						}
					}
				}
			}
		}
	}

	selectedItem = nullptr;
}

void GameSystemUserControl::SetSelectedItem(ISelectable* item)
{
	if (item)
	{
		selectedItem = item;
		selectedItem->Select();

		/*For the physics project. When a gameobject is selected, change color of meshes to red.*/
		Vector4D red = Vector4D(1.0f, 0.0f, 0.0f, 1.0f);
		Entity* go = dynamic_cast<Entity*>(item);
		if (go)
		{
			std::vector<IMeshComponent*> mCs;
			if (go->GetComponents(mCs))
			{
				for (IMeshComponent* mc : mCs)
				{
					std::vector<IMesh*> meshes;
					if (mc->GetMeshes(meshes))
					{
						for (IMesh* mesh : meshes)
						{
							mesh->SetDiffuse(red);
						}
					}
				}
			}
		}
	}
}

const bool GameSystemUserControl::IsActive()
{
	return this->isActive;
}

void GameSystemUserControl::Activate(const bool& value)
{
	this->isActive = value;
}

void GameSystemUserControl::ToggleWindowMode()
{
	if (isWindowMaximized)
	{
		glfwRestoreWindow(window);
	}
	else 
	{
		//glfwMaximizeWindow(window);

		// get resolution of monitor
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		// switch to full screen
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, 0);
	}

	isWindowMaximized = !isWindowMaximized;
}

void GameSystemUserControl::ToggleColliders()
{
	toggleColliders = !toggleColliders;
}

void GameSystemUserControl::NextLevel()
{
	moveNextLevel = true;
}

void GameSystemUserControl::PreviousLevel()
{
	movePreviousLevel = true;
}

GameSystemUserControl::GameSystemUserControl()
{
	this->isActive = true;
	window = 0;
	selectedLight = NULL;
	selectedGameObject = NULL;
	gameConfig = NULL;
	lastSelection = LAST_SELECTED_GAMEOBJECT;
	lastMouseXPosition = 0.0f;
	lastMouseYPosition = 0.0f;
	captureInput = true;
}

GameSystemUserControl::~GameSystemUserControl()
{
	if (systemVertexAnimation)
	{
		delete systemVertexAnimation;
		systemVertexAnimation = nullptr;
	}
}

const bool GameSystemUserControl::Initialize(ParameterContainer& input)
{
	Parameter p;
	bool rc = input.FindParameterByName("window", p);
	if (!rc) return false;

	this->window = (GLFWwindow*)p.valuePtr;
	if (!this->window) return false;

	rc = input.FindParameterByName("gameconfig", p);
	if (!rc) return false;

	this->gameConfig = (GameConfig*)p.valuePtr;
	if (!this->gameConfig) return false;

	SetGLCallbacks();

	SelectControllable(GameSystemUserControl::ControllableTypes_e::Camera);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void GameSystemUserControl::SetScene(Scene* scene)
{
	this->selectedScene = scene;
}

const bool GameSystemUserControl::IsCapturingInput()
{
	return this->captureInput;
}

void GameSystemUserControl::ToggleInputCapture(const bool& value)
{
	this->captureInput = value;
}

void GameSystemUserControl::BindKeyboardPressCallback(std::function<void(const KeyPress_s&)> fn)
{
	if (!fn) return;

	this->keypressCallback = fn;
}

void GameSystemUserControl::BindMouseClickCallback(std::function<void(const MouseKey_s&)> fn)
{
	if (!fn) return;

	this->mousepressCallback = fn;
}

void GameSystemUserControl::BindMouseMoveCallback(std::function<void(const MouseMove_s&)> fn)
{
	if (!fn) return;

	this->mousemoveCallback = fn;
}

const bool GameSystemUserControl::IsUpdateComplete()
{
	return this->isUpdateComplete;
}

void* GameSystemUserControl::GetInstance()
{
	if (!systemUserControl)
	{
		systemUserControl = new GameSystemUserControl();
	}

	return systemUserControl;
}

void* GameSystemUserControl::GetInstance(unsigned int numberOfArguments, ...)
{
	return GetInstance();
}

void GameSystemUserControl::AsyncProcessKeyPress()
{
	if (!captureInput) return;

	isMouseLeftClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

	//Move selected
	{
		Move();
	}

	//Leave window	
	if (glfwGetKey(window, gameConfig->keyBindings->Exit) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void GameSystemUserControl::AsyncProcessMousePress()
{
	if (!captureInput) return;

	isMouseLeftClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	isMouseRightClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}

void GameSystemUserControl::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, gameConfig->keyBindings->Pause) == GLFW_PRESS)
	{
		ToggleInputCapture();

		if (keypressCallback)
		{
			ISystemUserControl::KeyPress_s key_event;
			key_event.KeyID = gameConfig->keyBindings->Special[0];
			key_event.action = "toggle_input_capture";
			keypressCallback(key_event);
		}
	}

	if (!captureInput) return;	
		
	isShiftPressed = mods == GLFW_MOD_SHIFT;

	if (glfwGetKey(window, gameConfig->keyBindings->Jump) == GLFW_PRESS)
	{
		if (keypressCallback)
		{
			ISystemUserControl::KeyPress_s key_event;
			key_event.KeyID = gameConfig->keyBindings->Jump;
			key_event.action = "start_simulation";
			keypressCallback(key_event);
		}
	}

	if (glfwGetKey(window, gameConfig->keyBindings->InvisibilityVisionMode) == GLFW_PRESS)
	{
		if (keypressCallback)
		{
			KeyPress_s nfo;
			nfo.KeyID = GLFW_KEY_N;
			nfo.action = "player_night_vision";

			keypressCallback(nfo);
		}
	}

	if (glfwGetKey(window, gameConfig->keyBindings->SelectKey[1]) == GLFW_PRESS)
	{
		if (keypressCallback)
		{
			KeyPress_s nfo;
			nfo.KeyID = gameConfig->keyBindings->SelectKey[1];
			nfo.action = "selected_weapon_1";

			keypressCallback(nfo);
		}
	}
	if (glfwGetKey(window, gameConfig->keyBindings->SelectKey[2]) == GLFW_PRESS)
	{
		if (keypressCallback)
		{
			KeyPress_s nfo;
			nfo.KeyID = gameConfig->keyBindings->SelectKey[2];
			nfo.action = "selected_weapon_2";

			keypressCallback(nfo);
		}
	}
}

void GameSystemUserControl::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!captureInput) return;

	if (selectedScene)
	{
		ICamera* camera = selectedScene->GetCamera();
		if (camera)
		{
			float fov = camera->GetFOV();

			//Zoom In (Wheel Up)
			if (yoffset > 0)
			{
				fov -= 5 * 0.1f;
			}
			//Zoom In (Wheel Down)
			else
			{
				fov += 5 * 0.1f;
			}

			camera->SetFOV(fov);
		}
	}	
}

void GameSystemUserControl::MouseMoveCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!captureInput) return;

	if (mousemoveCallback)
	{
		MouseMove_s mm;
		mm.xoffset = xoffset;
		mm.yoffset = yoffset;
		mm.sensitivity = gameConfig->controlOptions.mouseSensitivity;

		mousemoveCallback(mm);
	}
	return;

	float deltaX = (float)(lastMouseXPosition - xoffset);
	float deltaY = (float)(lastMouseYPosition - yoffset);

	if (selectedScene)
	{
		ICamera* camera = selectedScene->GetCamera();

		if (camera->GetType() == "CameraLockedAt")
		{
			IRigidbody* cameraTarget = ((CameraLockedAt*)camera)->GetRigidbody();
			if (cameraTarget)
			{
				Quaternion currentRotation = cameraTarget->GetRotation();
				
				Quaternion qX = MathHelper::EulerAnglesToQuaternion(Vector3D(deltaY * gameConfig->controlOptions.mouseSensitivity, 0.0f, 0.0f));
				currentRotation *= qX;
				currentRotation = currentRotation * qX;

				Quaternion qY = MathHelper::EulerAnglesToQuaternion(Vector3D(0.0f, deltaX * gameConfig->controlOptions.mouseSensitivity, 0.0f));
				currentRotation *= qY;
				currentRotation = currentRotation * qY;

				cameraTarget->SetRotation(currentRotation);


				//MoveSelectable();
			}
			else
			{
				//Something is wrong. A CameraLockedAt must have a locking object.
			}
		}
		else
		{
			//Another type of camera
		}
	}

	lastMouseXPosition = (float)xoffset;
	lastMouseYPosition = (float)yoffset;
}

void GameSystemUserControl::MouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (!captureInput) return;

	//isMouseLeftClick = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//SetSelectedItem(FindEntityToCameraEye(10.0f));
		if (mousepressCallback)
		{
			MouseKey_s info;
			info.KeyID = MouseKey_s::MouseKey_e::Left;

			mousepressCallback(info);
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		//DisableSelectedItem();
		if (mousepressCallback)
		{
			MouseKey_s info;
			info.KeyID = MouseKey_s::MouseKey_e::Right;

			mousepressCallback(info);
		}
	}
}

void GameSystemUserControl::SaveState()
{
	saveState = true;	
}

void GameSystemUserControl::MoveCamera()
{
	if (glfwGetKey(window, gameConfig->keyBindings->MoveLeft) == GLFW_PRESS)
	{
		if (keypressCallback)
		{
			KeyPress_s nfo;
			nfo.KeyID = gameConfig->keyBindings->MoveLeft;
			nfo.action = "player_move_strafeleft";

			keypressCallback(nfo);
		}
	}
	if (glfwGetKey(window, gameConfig->keyBindings->MoveRight) == GLFW_PRESS)
	{
		if (keypressCallback)
		{
			KeyPress_s nfo;
			nfo.KeyID = gameConfig->keyBindings->MoveRight;
			nfo.action = "player_move_straferight";

			keypressCallback(nfo);
		}
	}
	if (glfwGetKey(window, gameConfig->keyBindings->MoveForward) == GLFW_PRESS)
	{
		if (keypressCallback)
		{
			KeyPress_s nfo;
			nfo.KeyID = gameConfig->keyBindings->MoveForward;
			nfo.action = "player_move_forward";

			keypressCallback(nfo);
		}
	}
	if (glfwGetKey(window, gameConfig->keyBindings->MoveBackward) == GLFW_PRESS)
	{
		if (keypressCallback)
		{
			KeyPress_s nfo;
			nfo.KeyID = gameConfig->keyBindings->MoveBackward;
			nfo.action = "player_move_backward";

			keypressCallback(nfo);
		}
	}
	if (glfwGetKey(window, gameConfig->keyBindings->MoveUp) == GLFW_PRESS)
	{
		if (keypressCallback)
		{
			KeyPress_s nfo;
			nfo.KeyID = gameConfig->keyBindings->MoveUp;
			nfo.action = "player_move_up";

			keypressCallback(nfo);
		}
	}
	if (glfwGetKey(window, gameConfig->keyBindings->MoveDown) == GLFW_PRESS)
	{
		if (keypressCallback)
		{
			KeyPress_s nfo;
			nfo.KeyID = gameConfig->keyBindings->MoveDown;
			nfo.action = "player_move_down";

			keypressCallback(nfo);
		}
	}
	if (glfwGetKey(window, gameConfig->keyBindings->Jump) == GLFW_PRESS)
	{
		if (keypressCallback)
		{
			KeyPress_s nfo;
			nfo.KeyID = gameConfig->keyBindings->Jump;
			nfo.action = "player_move_halt";

			keypressCallback(nfo);
		}
	}	
	return;

	const float ROTATIONFACTOR = 0.1f;
	Scene* scene = selectedScene;

	ICamera* camera = scene->GetCamera();

	if (camera->GetType() == "CameraLockedAt")
	{
		CameraLockedAt* cameraLockedAt = (CameraLockedAt*)camera;
		cameraLockedAt->SetMoveFactor(gameConfig->controlOptions.movementStep);

		//Movement
		{
			if (glfwGetKey(window, gameConfig->keyBindings->MoveLeft) == GLFW_PRESS)
			{
				cameraLockedAt->StrafeLeft();
			}
			if (glfwGetKey(window, gameConfig->keyBindings->MoveRight) == GLFW_PRESS)
			{
				cameraLockedAt->StrafeRight();
			}
			if (glfwGetKey(window, gameConfig->keyBindings->MoveForward) == GLFW_PRESS)
			{
				cameraLockedAt->MoveForward();
			}
			if (glfwGetKey(window, gameConfig->keyBindings->MoveBackward) == GLFW_PRESS)
			{
				cameraLockedAt->MoveBackward();
			}
		}

		MoveSelectable();
	}
	else if (camera->GetType() == "CameraFree")
	{
		CameraFree* cameraFree = ((CameraFree*)camera);
		if (camera)
		{
			Vector3D eyePosition = camera->GetEyePosition();

			if (glfwGetKey(window, gameConfig->keyBindings->SpeedLeft) == GLFW_PRESS)
			{
				eyePosition.x -= gameConfig->controlOptions.movementStep;
			}
			if (glfwGetKey(window, gameConfig->keyBindings->SpeedRight) == GLFW_PRESS)
			{
				eyePosition.x += gameConfig->controlOptions.movementStep;
			}

			Vector3D currentPosition = camera->GetTargetPosition();

			if (glfwGetKey(window, gameConfig->keyBindings->MoveLeft) == GLFW_PRESS)
			{
				eyePosition.x -= gameConfig->controlOptions.movementStep;
				currentPosition.x -= gameConfig->controlOptions.movementStep;
			}
			if (glfwGetKey(window, gameConfig->keyBindings->MoveRight) == GLFW_PRESS)
			{
				eyePosition.x += gameConfig->controlOptions.movementStep;
				currentPosition.x += gameConfig->controlOptions.movementStep;
			}
			if (glfwGetKey(window, gameConfig->keyBindings->MoveUp) == GLFW_PRESS)
			{
				eyePosition.y -= gameConfig->controlOptions.movementStep;
				currentPosition.y -= gameConfig->controlOptions.movementStep;
			}
			if (glfwGetKey(window, gameConfig->keyBindings->MoveDown) == GLFW_PRESS)
			{
				eyePosition.y += gameConfig->controlOptions.movementStep;
				currentPosition.y += gameConfig->controlOptions.movementStep;
			}

			if (glfwGetKey(window, gameConfig->keyBindings->MoveForward) == GLFW_PRESS)
			{
				eyePosition.z -= gameConfig->controlOptions.movementStep;
				currentPosition.z -= gameConfig->controlOptions.movementStep;
			}
			if (glfwGetKey(window, gameConfig->keyBindings->MoveBackward) == GLFW_PRESS)
			{
				eyePosition.z += gameConfig->controlOptions.movementStep;
				currentPosition.z += gameConfig->controlOptions.movementStep;
			}

			camera->SetEyePosition(eyePosition);

			/*if (currentPosition != camera->GetTargetPosition())
			{
				camera->SetTarget(currentPosition);
			}*/
		}
	}
}

void GameSystemUserControl::MoveSound()
{
	if (!selectedSound) return;

	//if (selectedSound->HasFlag(FMOD_3D))
	{
		if (glfwGetKey(window, gameConfig->keyBindings->MoveLeft) == GLFW_PRESS)
		{
			Vector3D currentPosition = systemSound->GetPosition(selectedSound->GetName());

			currentPosition.x -= 0.1f;
			systemSound->SetPosition(selectedSound->GetName(), currentPosition);

			printf("Moved sound [%s] to (%.2f, %.2f, %.2f)\n", selectedSound->GetName().c_str(), currentPosition.x, currentPosition.y, currentPosition.z);
		}

		if (glfwGetKey(window, gameConfig->keyBindings->MoveRight) == GLFW_PRESS)
		{
			Vector3D currentPosition = systemSound->GetPosition(selectedSound->GetName());

			currentPosition.x += 0.1f;
			systemSound->SetPosition(selectedSound->GetName(), currentPosition);

			printf("Moved sound [%s] to (%.2f, %.2f, %.2f)\n", selectedSound->GetName().c_str(), currentPosition.x, currentPosition.y, currentPosition.z);
		}

		if (glfwGetKey(window, gameConfig->keyBindings->MoveUp) == GLFW_PRESS)
		{
			Vector3D currentPosition = systemSound->GetPosition(selectedSound->GetName());

			currentPosition.y -= 0.1f;
			systemSound->SetPosition(selectedSound->GetName(), currentPosition);

			printf("Moved sound [%s] to (%.2f, %.2f, %.2f)\n", selectedSound->GetName().c_str(), currentPosition.x, currentPosition.y, currentPosition.z);
		}

		if (glfwGetKey(window, gameConfig->keyBindings->MoveDown) == GLFW_PRESS)
		{
			Vector3D currentPosition = systemSound->GetPosition(selectedSound->GetName());

			currentPosition.y += 0.1f;
			systemSound->SetPosition(selectedSound->GetName(), currentPosition);

			printf("Moved sound [%s] to (%.2f, %.2f, %.2f)\n", selectedSound->GetName().c_str(), currentPosition.x, currentPosition.y, currentPosition.z);
		}

		if (glfwGetKey(window, gameConfig->keyBindings->MoveForward) == GLFW_PRESS)
		{
			Vector3D currentPosition = systemSound->GetPosition(selectedSound->GetName());

			currentPosition.z -= 0.1f;
			systemSound->SetPosition(selectedSound->GetName(), currentPosition);

			printf("Moved sound [%s] to (%.2f, %.2f, %.2f)\n", selectedSound->GetName().c_str(), currentPosition.x, currentPosition.y, currentPosition.z);
		}

		if (glfwGetKey(window, gameConfig->keyBindings->MoveBackward) == GLFW_PRESS)
		{
			Vector3D currentPosition = systemSound->GetPosition(selectedSound->GetName());

			currentPosition.z += 0.1f;
			systemSound->SetPosition(selectedSound->GetName(), currentPosition);

			printf("Moved sound [%s] to (%.2f, %.2f, %.2f)\n", selectedSound->GetName().c_str(), currentPosition.x, currentPosition.y, currentPosition.z);
		}
	}
}

void GameSystemUserControl::MoveLight()
{
	if (!selectedLight) return;

	std::string lightType = selectedLight->GetType();

	if (glfwGetKey(window, gameConfig->keyBindings->MoveForward) == GLFW_PRESS)
	{
		Vector3D p = Vector3D(0.0f, 0.0f, 0.1f);
		if (lightType == "LightPoint")
		{
			((LightPoint*)selectedLight)->SetPosition(((LightPoint*)selectedLight)->GetPosition() - p);
		}
		else if (lightType == "LightSpot")
		{
			((LightSpot*)selectedLight)->SetPosition(((LightSpot*)selectedLight)->GetPosition() - p);
		}
	}
	if (glfwGetKey(window, gameConfig->keyBindings->MoveBackward) == GLFW_PRESS)
	{
		Vector3D p = Vector3D(0.0f, 0.0f, 0.1f);
		if (lightType == "LightPoint")
		{
			((LightPoint*)selectedLight)->SetPosition(((LightPoint*)selectedLight)->GetPosition() + p);
		}
		else if (lightType == "LightSpot")
		{
			((LightSpot*)selectedLight)->SetPosition(((LightSpot*)selectedLight)->GetPosition() + p);
		}
	}
	if (glfwGetKey(window, gameConfig->keyBindings->MoveLeft) == GLFW_PRESS)
	{
		Vector3D p = Vector3D(0.1f, 0.0f, 0.0f);
		if (lightType == "LightPoint")
		{
			((LightPoint*)selectedLight)->SetPosition(((LightPoint*)selectedLight)->GetPosition() - p);
		}
		else if (lightType == "LightSpot")
		{
			((LightSpot*)selectedLight)->SetPosition(((LightSpot*)selectedLight)->GetPosition() - p);
		}
	}
	if (glfwGetKey(window, gameConfig->keyBindings->MoveRight) == GLFW_PRESS)
	{
		Vector3D p = Vector3D(0.1f, 0.0f, 0.0f);
		if (lightType == "LightPoint")
		{
			((LightPoint*)selectedLight)->SetPosition(((LightPoint*)selectedLight)->GetPosition() + p);
		}
		else if (lightType == "LightSpot")
		{
			((LightSpot*)selectedLight)->SetPosition(((LightSpot*)selectedLight)->GetPosition() + p);
		}
	}
	if (glfwGetKey(window, gameConfig->keyBindings->MoveUp) == GLFW_PRESS)
	{
		Vector3D p = Vector3D(0.0f, 0.1f, 0.0f);
		if (lightType == "LightPoint")
		{
			((LightPoint*)selectedLight)->SetPosition(((LightPoint*)selectedLight)->GetPosition() + p);
		}
		else if (lightType == "LightSpot")
		{
			((LightSpot*)selectedLight)->SetPosition(((LightSpot*)selectedLight)->GetPosition() + p);
		}
	}
	if (glfwGetKey(window, gameConfig->keyBindings->MoveDown) == GLFW_PRESS)
	{
		Vector3D p = Vector3D(0.0f, 0.1f, 0.0f);
		if (lightType == "LightPoint")
		{
			((LightPoint*)selectedLight)->SetPosition(((LightPoint*)selectedLight)->GetPosition() - p);
		}
		else if (lightType == "LightSpot")
		{
			((LightSpot*)selectedLight)->SetPosition(((LightSpot*)selectedLight)->GetPosition() - p);
		}
	}
}

void GameSystemUserControl::Move()
{
	switch (selectedControllableType)
	{
	case GameSystemUserControl::ControllableTypes_e::Camera:
		MoveCamera();
		break;
	case GameSystemUserControl::ControllableTypes_e::GameObject:
		MoveGameObject();
		break;
	case GameSystemUserControl::ControllableTypes_e::Light:
		MoveLight();
		break;
	case GameSystemUserControl::ControllableTypes_e::Sound:
		MoveSound();
		break;
	case GameSystemUserControl::ControllableTypes_e::ControllableCount:
		break;
	default:
		break;
	}
}

void GameSystemUserControl::AddNewObject(ICloneable* obj)
{
	if (!selectedScene) return;

	Entity* go = dynamic_cast<Entity*>(obj);
	if (go)
	{
		selectedScene->Append(go);
		return;
	}

	ILight* light = dynamic_cast<ILight*>(obj);
	if (light)
	{
		selectedScene->Append(light);
		return;
	}
}
void GameSystemUserControl::RemoveObject(ICloneable* obj)
{
	if (!selectedScene) return;

	Entity* go = dynamic_cast<Entity*>(obj);
	if (go)
	{
		selectedScene->Remove(go);
		return;
	}

	ILight* light = dynamic_cast<ILight*>(obj);
	if (light)
	{
		selectedScene->Remove(light);
		return;
	}
}

void GameSystemUserControl::SetGLCallbacks()
{
	glfwSetMouseButtonCallback
	(
		window,
		[](GLFWwindow* window, int button, int action, int mods)
		{
			systemUserControl->MouseClickCallback(window, button, action, mods);
		}
	);
	glfwSetKeyCallback
	(
		window,
		[](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			systemUserControl->KeyboardCallback(window, key, scancode, action, mods);
		}
	);

	glfwSetScrollCallback
	(
		window,
		[](GLFWwindow* window, double xoffset, double yoffset)
		{
			systemUserControl->MouseScrollCallback(window, xoffset, yoffset);
		}
	);

	glfwSetCursorPosCallback
	(
		window,
		[](GLFWwindow* window, double xoffset, double yoffset)
		{
			systemUserControl->MouseMoveCallback(window, xoffset, yoffset);
		}
	);

	/*glfwSetWindowSizeCallback
	(
		window,
		[](GLFWwindow* window, int width, int height)
		{
			world->GetVideoSystem()->SetWindowSize(width, height);
			glViewport(0, 0, world->GetVideoSystem()->windowWidth, world->GetVideoSystem()->windowHeight);
		}
	);*/
}

void GameSystemUserControl::ControlAnimatedCharacters()
{
	if (!selectedItem) return;

	Entity* go = dynamic_cast<Entity*>(selectedItem);
	//if (animationProject->IsAnimatedCharacter(go))
	{
		if (glfwGetKey(window, gameConfig->keyBindings->Action0) == GLFW_PRESS)
		{
			moveCharacter = true;
			movement_action = GameVertexAnimationSystem::Action_e::Action;
		}
		else if (glfwGetKey(window, gameConfig->keyBindings->RotateLeft) == GLFW_PRESS)
		{
			moveCharacter = true;
			movement_action = GameVertexAnimationSystem::Action_e::RotateLeft;
		}
		else if (glfwGetKey(window, gameConfig->keyBindings->RotateRight) == GLFW_PRESS)
		{
			moveCharacter = true;
			movement_action = GameVertexAnimationSystem::Action_e::RotateRight;
		}
		else if (glfwGetKey(window, gameConfig->keyBindings->Jump) == GLFW_PRESS)
		{
			moveCharacter = true;
			movement_action = GameVertexAnimationSystem::Action_e::Jump;
		}
		else if (glfwGetKey(window, gameConfig->keyBindings->MoveLeft) == GLFW_PRESS)
		{
			moveCharacter = true;
			if (isShiftPressed)
			{				
				movement_action = GameVertexAnimationSystem::Action_e::RollLeft;
			}
			else
			{
				movement_action = GameVertexAnimationSystem::Action_e::StrafeLeft;
			}
		}
		else if (glfwGetKey(window, gameConfig->keyBindings->MoveRight) == GLFW_PRESS)
		{
			moveCharacter = true;			
			if (isShiftPressed)
			{
				movement_action = GameVertexAnimationSystem::Action_e::RollRight;
			}
			else
			{
				movement_action = GameVertexAnimationSystem::Action_e::StrafeRight;
			}
		}
		else if (glfwGetKey(window, gameConfig->keyBindings->MoveForward) == GLFW_PRESS)
		{
			moveCharacter = true;
			if (isShiftPressed)
			{
				movement_action = GameVertexAnimationSystem::Action_e::Run;
			}
			else
			{
				movement_action = GameVertexAnimationSystem::Action_e::Walk;
			}
		}
		else if (glfwGetKey(window, gameConfig->keyBindings->MoveBackward) == GLFW_PRESS)
		{
			moveCharacter = true;
			movement_action = GameVertexAnimationSystem::Action_e::WalkBackwards;
		}
		else
		{
			moveCharacter = false;
			movement_action = GameVertexAnimationSystem::Action_e::Idle;
		}
	}
}

Entity* GameSystemUserControl::FindEntityToCameraEye(const float& raycastLength)
{
	if (!selectedScene) return nullptr;

	ICamera* camera = selectedScene->GetCamera();

	//Ray
	Vector3D rayStart = camera->GetEyePosition();
	Vector3D rayDirection = camera->GetForward();

	IRigidbody* rb = nullptr;
	ICollider* collider = nullptr;
	std::map<float, Entity*> objectsInRange;
	if (selectedScene->GetEntityInCameraRange(raycastLength, objectsInRange))
	{
		for (auto element : objectsInRange)
		{
			rb = element.second->GetComponent<IRigidbody*>();
			if (rb)
			{
				collider = rb->GetCollider();
				if (collider)
				{
					if (collider->Raycast(rayStart, rayDirection, raycastLength))
					{
						/*it is the closest one because the map is ordered.*/
						return element.second;
					}
				}
			}
		}
	}

	return nullptr;
}

void GameSystemUserControl::Update(const FrameTime& frameTime)
{
	if (!this->isActive) return;
	this->isUpdateComplete = false;

	SetFrameTime(frameTime);

	AsyncProcessKeyPress();
	AsyncProcessMousePress();

	this->isUpdateComplete = true;
}
