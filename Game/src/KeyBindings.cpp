/***********************************************************************
* Project           : Game
* Author            : Felipe Bellini
* Date created      : Oct. 12 - 2019
* Licensed under:	: MIT

************************************************************************/
#include "KeyBindings.h"

KeyBindings::KeyBindings()
{
	memset(Special, NULL, 12u);
	memset(SelectKey, NULL, 10u);

	SelectNext = 0;

	GameObjectModifier = 0;
	LightModifier = 0;
	GeneralPurposeModifier = 0;
	SpeedForward = 0;
	SpeedBackward = 0;
	SpeedRight = 0;
	SpeedLeft = 0;
	MoveForward = 0;
	MoveLeft = 0;
	MoveRight = 0;
	MoveBackward = 0;
	MoveUp = 0;
	MoveDown = 0;
	StrafeLeft = 0;
	StrafeRight = 0;
	Jump = 0;
	Action0 = 0;
	Action1 = 0;
	Shoot = 0;
	Reload = 0;
	RotateLeft = 0;
	RotateRight = 0;
	ScaleUp = 0;
	ScaleDown = 0;
	SelectAxis = 0;
	SelectColorBit = 0;
	IncreaseLightAtten = 0;
	DecreaseLightAtten = 0;
	IncreaseLightQuadAtten = 0;
	DecreaseLightQuadAtten = 0;
	Pause = 0;
	InvisibilityVisionMode = 0;
	Exit = 0;
}

KeyBindings::~KeyBindings()
{
}

void KeyBindings::LoadKeys()
{
	for (auto& element : keyBindingMap)
	{
		if (element.first == "GameObjectModifier")			GameObjectModifier = element.second;
		if (element.first == "LightModifier")				LightModifier = element.second;
		if (element.first == "GeneralPurposeModifier")		GeneralPurposeModifier = element.second;
		if (element.first == "SpeedForward")				SpeedForward = element.second;
		if (element.first == "SpeedBackward")				SpeedBackward = element.second;
		if (element.first == "SpeedLeft")					SpeedLeft = element.second;
		if (element.first == "SpeedRight")					SpeedRight = element.second;
		if (element.first == "MoveForward")					MoveForward = element.second;
		if (element.first == "MoveRight")					MoveRight = element.second;
		if (element.first == "MoveLeft")					MoveLeft = element.second;
		if (element.first == "MoveBackward")				MoveBackward = element.second;
		if (element.first == "MoveUp")						MoveUp = element.second;
		if (element.first == "MoveDown")					MoveDown = element.second;
		if (element.first == "StrafeLeft")					StrafeLeft = element.second;
		if (element.first == "StrafeRight")					StrafeRight = element.second;
		if (element.first == "RotateLeft")					RotateLeft = element.second;
		if (element.first == "RotateRight")					RotateRight = element.second;
		if (element.first == "ScaleUp")						ScaleUp = element.second;
		if (element.first == "ScaleDown")					ScaleDown = element.second;
		if (element.first == "SelectAxis")					SelectAxis = element.second;
		if (element.first == "SelectColorBit")				SelectColorBit = element.second;
		if (element.first == "IncreaseLightAtten")			IncreaseLightAtten = element.second;
		if (element.first == "DecreaseLightAtten")			DecreaseLightAtten = element.second;
		if (element.first == "IncreaseLightQuadAtten")		IncreaseLightQuadAtten = element.second;
		if (element.first == "DecreaseLightQuadAtten")		DecreaseLightQuadAtten = element.second;
		if (element.first == "Action")						Action0 = element.second;
		if (element.first == "Jump")						Jump = element.second;
		if (element.first == "Exit")						Exit = element.second;
		if (element.first == "Pause")						Pause = element.second;
		if (element.first == "InvisibilityVisionMode")		InvisibilityVisionMode = element.second;
		if (element.first == "Action0")						Action0 = element.second;
		if (element.first == "Action1")						Action1 = element.second;

		if (element.first == "SelectKey0")					SelectKey[0] = element.second;
		if (element.first == "SelectKey1")					SelectKey[1] = element.second;
		if (element.first == "SelectKey2")					SelectKey[2] = element.second;
		if (element.first == "SelectKey3")					SelectKey[3] = element.second;
		if (element.first == "SelectKey4")					SelectKey[4] = element.second;
		if (element.first == "SelectKey5")					SelectKey[5] = element.second;
		if (element.first == "SelectKey6")					SelectKey[6] = element.second;
		if (element.first == "SelectKey7")					SelectKey[7] = element.second;
		if (element.first == "SelectKey8")					SelectKey[8] = element.second;
		if (element.first == "SelectKey9")					SelectKey[9] = element.second;
		if (element.first == "SelectNext")					SelectNext = element.second;

		if (element.first == "Special0")					Special[0] = element.second;
		if (element.first == "Special1")					Special[1] = element.second;
		if (element.first == "Special2")					Special[2] = element.second;
		if (element.first == "Special3")					Special[3] = element.second;
		if (element.first == "Special4")					Special[4] = element.second;
		if (element.first == "Special5")					Special[5] = element.second;
		if (element.first == "Special6")					Special[6] = element.second;
		if (element.first == "Special7")					Special[7] = element.second;
		if (element.first == "Special8")					Special[8] = element.second;
		if (element.first == "Special9")					Special[9] = element.second;
		if (element.first == "Special10")					Special[10] = element.second;
		if (element.first == "Special11")					Special[11] = element.second;
	}
}
