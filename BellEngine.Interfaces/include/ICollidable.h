//#pragma once
//#include "IRigidbody.h"
//
//class ICollidable
//{
//protected:
//	IRigidbody* rigidbody;
//
//	ICollidable() : rigidbody(nullptr) {};
//
//public:
//	virtual ~ICollidable() {};
//
//	bool AttachRigidBody(IRigidbody* rigidbody)
//	{
//		this->rigidbody = rigidbody;
//		return rigidbody != nullptr;
//	}
//	IRigidbody* GetRigidbody()
//	{
//		return this->rigidbody;
//	}
//};