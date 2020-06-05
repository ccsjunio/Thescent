/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <MathLibrary.h>
#include <vector>
#include <map>
#include "IIdentifiable.h"

class ObjectGroup
{
private:
	std::map<std::string, Vector3D> originalPositions;
	std::map<std::string, Quaternion> originalRotationsQ;

	void UpdatePositionByGain(Vector3D position);
	void UpdateRotation();
	void UpdateScale();
	void UpdateSpeed();
	void UpdateAcceleration();

	bool IsDiffThanZero(Vector3D input);

public:
	std::string name;
	Vector3D position;
	Vector3D speed;
	Vector3D acceleration;
	Vector3D rotation;
	Vector3D scale;
	std::vector<IIdentifiable*> objects;
	bool isSelected;
	IIdentifiable* referenceObject;

	ObjectGroup();
	~ObjectGroup();

	void GetObjects(std::vector<IIdentifiable*>& objects);
	void Register(IIdentifiable* object);
	void Update(float deltaTime);
	void SetReferenceObject(IIdentifiable* object);

	void Rotate(Vector3D rotation);
	void Reposition(Vector3D position);
	void Rescale(Vector3D scale);
	void Accelerate(Vector3D acceleration);
	void Impulse(Vector3D speed);
	void UpdateGroupData();
	void Select(bool option);
	std::string GetName();
};

