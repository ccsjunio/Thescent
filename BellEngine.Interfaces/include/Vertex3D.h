#pragma once
#include <MathLibrary.h>
#include "ICloneable.h"

#define MAX_BONE_COUNT_PER_VERTEX 4

class Vertex3D : public ICloneable
{
public:
	unsigned int index;

	//For models
	float x, y, z, w;									//Position coordinates	
	float r, g, b, a;									//Color coordinates  - a = alpha (transparency)
	float nx, ny, nz, nw;								//Normals coordinates
	float u0, v0, u1, v1;								//Textures coordinates
	
	//For bump maps
	float tx, ty, tz, tw;								//Tangent
	float bx, by, bz, bw;								//Bi-normal

	//For mesh animations
	float boneID[MAX_BONE_COUNT_PER_VERTEX];			//Which bone impacts this vertex
	float boneWeights[MAX_BONE_COUNT_PER_VERTEX];		//How much does each bone weight

	Vertex3D()
	{
		bx = by = bz = 0.0f;
		bw = 1.0f;
		tx = ty = tz = 0.0f;
		tw = 1.0f;
		boneID[0] = boneID[1] = boneID[2] = boneID[3] = 0.0f;
		boneWeights[0] = boneWeights[1] = boneWeights[2] = boneWeights[3] = 0.0f;
				
		x = y =	z = w = 1.0f;
		r = g = b = a = 1.0f;
		nx = ny = nz = nw = 1.0f;
		u0 = v0 = 1.0f;
		u1 = v1 = 1.0f;

		index = 0;
	}

	Vector3D GetVec3()
	{
		return Vector3D(x, y, z);
	}
	ICloneable* Clone()
	{
		Vertex3D* clone = new Vertex3D();

		clone->bx = bx;
		clone->by = by;
		clone->bz = bz;
		clone->bw = bw;
		clone->tx = tx;
		clone->ty = ty;
		clone->tz = tz;
		clone->tw = tw;
		
		for (unsigned int i = 0; i < 4u; i++)
		{
			clone->boneID[i] = boneID[i];
			clone->boneWeights[i] = boneWeights[i];
		}

		clone->x = x;
		clone->y = y;
		clone->z = z;
		clone->w = w;

		clone->r = r;
		clone->g = g;
		clone->b = b;
		clone->a = a;
		
		clone->nx = nx;
		clone->ny = ny;
		clone->nz = nz;
		clone->nw = nw;

		clone->u0 = u0;
		clone->v0 = v0;
		clone->u1 = u1;
		clone->v1 = v1;

		clone->index = index;

		return clone;
	}
};