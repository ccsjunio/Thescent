/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <string>
#include <Vertex3D.h>

struct ModelDrawInfo
{	
	std::string name;
	std::string path;
	unsigned int VAO_ID = 0;
	unsigned int vertexBufferID = 0;
	unsigned int vertexBuffer_Start_Index = 0;
	unsigned int indexBufferID = 0;
	unsigned int indexBuffer_Start_Index = 0;
	std::vector<Vertex3D> vertices3D;
	std::vector<unsigned int> indices;
};