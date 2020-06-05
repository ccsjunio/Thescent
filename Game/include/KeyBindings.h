/***********************************************************************
* Project           : Game
* Author            : Felipe Bellini
* Date created      : Oct. 12 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include "BaseKeyBindings.h"

class KeyBindings : public BaseKeyBindings
{
public:
	 int SelectKey[10];
	 int Special[12];
	 int SelectNext;

	 int GameObjectModifier;
	 int LightModifier;
	 int GeneralPurposeModifier;
	 int SpeedForward;
	 int SpeedBackward;
	 int SpeedRight;
	 int SpeedLeft;
	 int MoveForward;
	 int MoveLeft;
	 int MoveRight;
	 int MoveBackward;
	 int MoveUp;
	 int MoveDown;
	 int StrafeLeft;
	 int StrafeRight;
	 int Jump;
	 int Action0;
	 int Action1;
	 int Shoot;
	 int Reload;
	 int RotateLeft;
	 int RotateRight;
	 int ScaleUp;
	 int ScaleDown;
	 int SelectAxis;
	 int SelectColorBit;
	 int IncreaseLightAtten;
	 int DecreaseLightAtten;
	 int IncreaseLightQuadAtten;
	 int DecreaseLightQuadAtten;
	 int Pause;
	 int InvisibilityVisionMode;
	 int Exit;
	
	KeyBindings();
	~KeyBindings();
	virtual void LoadKeys();
};

