#pragma once
#include <GLCommon.h>

struct LightShaderStruct
{
public:
	GLint positionUL;	//position of the light		
	GLint diffuseUL;	//"Solid" color of the light
	GLint specularUL;	//rgb = highlight colour, w = power
	GLint attenUL;		//x = constant, y = linear, z = quadratic, w = DistanceCutOff
	GLint directionUL;	//For spot and directional lights
	GLint param1UL;	//x = lightType, y = inner angle, z = outer angle, w = TBD
					//0 = pointlight
					//1 = spot light
					//2 = directional light
	GLint param2UL;	//x = 0 for off, 1 for on
};