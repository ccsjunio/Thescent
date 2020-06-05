#pragma once
#include <ITransform.h>

#ifdef __linux__ 
#include <mutex>
#elif _WIN32
#define NOMINMAX
#include <windows.h>
#else

#endif

class Transform : public ITransform
{
protected:
#ifdef __linux__ 
	std::mutex mutex;
#elif _WIN32
	CRITICAL_SECTION lock;
#else

#endif

	Vector3D orientation;
	Vector3D up;
	Vector3D position;
	Vector3D scale;
	Quaternion rotation;

public:
	Transform() : 
		position(Vector3D()), 
		scale(Vector3D()), 
		rotation(Quaternion()), 
		orientation(Vector3D()),
		up(Vector3D())
	{
#ifdef __linux__ 

#elif _WIN32
		InitializeCriticalSection(&lock);
#endif
	};
	~Transform() 
	{
#ifdef __linux__ 

#elif _WIN32
		DeleteCriticalSection(&lock);
#endif
	};
	
	void SetOrientation(const Vector3D& orientation)
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		this->orientation = orientation;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif
	}
	void SetUp(const Vector3D& up)
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		this->up = up;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif
	}
	void SetPosition(const Vector3D& position)
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		this->position = position;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif
	}
	void SetScale(const Vector3D& scale)
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		this->scale = scale;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif
	}
	void SetRotation(const Quaternion& rotation)
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		this->rotation = rotation;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif
	}
	const Vector3D GetOrientation()
	{
		Vector3D v;

#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		v = orientation;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif

		return v;
	}
	const Vector3D GetUp() 
	{
		Vector3D v;

#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		v = up;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif

		return v;
	}
	const Vector3D GetPosition() 
	{
		Vector3D v;

#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		v = position;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif

		return v;
	}
	const Vector3D GetScale() 
	{
		Vector3D v;

#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		v = scale;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif

		return v;
	}
	const Quaternion GetRotation()
	{
		Quaternion q;

#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		q = rotation;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif

		return q;
	}
	ICloneable* Clone() 
	{ 
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		Transform* clone = new Transform();
		clone->orientation = this->orientation;
		clone->up = this->up;
		clone->position = this->position;
		clone->rotation = this->rotation;
		clone->scale = this->scale;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif

		return clone;
	};
};