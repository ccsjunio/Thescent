#pragma once
#include <map>
#include <Scene.h>

#ifdef __linux__ 
#include <mutex>
#elif _WIN32
#define NOMINMAX
#include <windows.h>
#else

#endif

extern "C"
{
	#include <Lua/lua.h>
	#include <Lua/lauxlib.h>
	#include <Lua/lualib.h>
}

struct ScriptKVP
{
private:
#ifdef __linux__ 
	std::mutex mutex;
#elif _WIN32
	CRITICAL_SECTION lock;
#else

#endif

	std::string name, content;

public:
	ScriptKVP()
	{
#ifdef __linux__ 

#elif _WIN32
		InitializeCriticalSection(&lock);
#endif
	};
	ScriptKVP(const std::string& name, const std::string& content)
	{
#ifdef __linux__ 

#elif _WIN32
		InitializeCriticalSection(&lock);
#endif

		SetName(name);
		SetContent(content);
	};
	~ScriptKVP()
	{
#ifdef __linux__ 

#elif _WIN32
		DeleteCriticalSection(&lock);
#endif
	};

	void SetName(const std::string& value)
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		this->name = value;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif
	}
	const std::string GetName()
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		std::string value = this->name;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif

		return value;
	}

	void SetContent(const std::string& value)
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		this->content = value;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif
	}
	const std::string GetContent()
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		std::string value = this->content;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif

		return value;
	}
};

struct LuaWorldInfo
{
public:
	lua_State* luaState;
	Scene* scene;
	std::vector<ScriptKVP*> scripts;
	std::vector<std::string> disposeList;
};