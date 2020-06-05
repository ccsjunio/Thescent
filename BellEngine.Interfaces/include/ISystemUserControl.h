#pragma once
#include <ISystem.h>
#include <functional>
#include <ParameterContainer.h>
#include <Scene.h>

class ISystemUserControl : public ISystem
{
protected:
	ISystemUserControl() {};

public:
	struct KeyPress_s
	{
	public:
		char KeyID;
		std::string action;
		KeyPress_s() : KeyID(0) {};
		~KeyPress_s() {};
	};
	struct MouseKey_s
	{
	public:
		enum class MouseKey_e { Left, Right, Wheel };
		MouseKey_e KeyID;
		MouseKey_s() : KeyID(MouseKey_e::Left) {};
		~MouseKey_s() {};
	};
	struct MouseMove_s
	{
	public:
		double xoffset, yoffset, sensitivity;
		MouseMove_s() : xoffset(0.0), yoffset(0.0), sensitivity(0.0) {};
		~MouseMove_s() {};
	};

	virtual ~ISystemUserControl() {};

	virtual const bool Initialize(ParameterContainer& input) = 0;
	virtual void SetScene(Scene* scene) = 0;
	virtual const bool IsCapturingInput() = 0;
	virtual void ToggleInputCapture(const bool& value) = 0;
	virtual void BindKeyboardPressCallback(std::function<void(const KeyPress_s&)> fn) = 0;
	virtual void BindMouseClickCallback(std::function<void(const MouseKey_s&)> fn) = 0;
	virtual void BindMouseMoveCallback(std::function<void(const MouseMove_s&)> fn) = 0;
};