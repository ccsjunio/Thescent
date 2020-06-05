#pragma once
#include <ISystem.h>
#include <functional>
#include <ParameterContainer.h>

class ISystemGUI : public ISystem
{
private:

protected:
	ISystemGUI() {};

public:
	struct ScreenProperties_s
	{
		unsigned int screenWidth, screenHeight;
	};

	struct GUIMessage_s
	{
	public:
		unsigned int contextFlags;
		std::string text;
		double duration, timeacc;
		unsigned int renderAs;
		GUIMessage_s() : contextFlags(0u), duration(0.0f), timeacc(0.0f), renderAs(0u) {};
		~GUIMessage_s() {};
	};
	struct GUIEvent_s
	{
	public:
		unsigned int eventID;
		std::string eventName, eventDescription;
		GUIEvent_s() : eventID(0u) {};
		~GUIEvent_s() {};
	};

	virtual ~ISystemGUI() {};

	virtual void SetScreenProperties(const ISystemGUI::ScreenProperties_s& properties) = 0;
	virtual const bool PushMessage(const unsigned int& contextFlags, const std::string& text, const double& duration, const unsigned int& renderAs = 0u) = 0;
	virtual const bool Initialize(ParameterContainer& input) = 0;
	virtual void BindGUICallback(std::function<void(const GUIEvent_s&)> fn) = 0;
	virtual void HandleInput(const bool& option) = 0;	
};