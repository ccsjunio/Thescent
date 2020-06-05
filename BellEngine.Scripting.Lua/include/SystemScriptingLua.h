#pragma once

extern "C" 
{
	#include <Lua/lua.h>
	#include <Lua/lauxlib.h>
	#include <Lua/lualib.h>
}

#include <string>
#include <vector>
#include <map>
#include <atomic>
#include <ISystemScripting.h>
#include "LuaWorldInfo.h"
#include <IScriptedAnimation.h>
#include <functional>
#include <IDisposable.h>

using LuaFunctionType = int(lua_State*);

class SystemScriptingLua : public ISystemScripting, public IDisposable
{
private:
	std::atomic<bool> isActive, isUpdateComplete;
	static std::map<std::string, LuaWorldInfo> luaWorldInfos;
	static std::map<std::string, /*std::function<LuaFunctionType>*/ LuaFunctionType*> functionPointers;
	static SystemScriptingLua* scriptingSystem;
	Scene* scene;
	IFactory<IScriptedAnimation>* scriptedAnimationFactory;
	IFactory<IScript>* scriptFactory;

	void InitializeLuaRuntime();
	void DisposeLuaRuntime();
	const bool DeleteScript(const std::string& scriptName);
	static const std::string GetLuaErrorString(const int& error);

public:	
	SystemScriptingLua();
	~SystemScriptingLua();

	const bool PlayAnimation(const std::string& name, const std::string& sceneName, const bool& requestedByUserInput = false);
	const bool IsUpdateComplete();
	IFactory<IScriptedAnimation>* GetScriptedAnimationFactory();
	IFactory<IScript>* GetScriptFactory();
	const bool IsActive();
	void Activate(const bool& value = true);
	void HandleSerialAnimation(IScriptedAnimation* animation, const bool& requestedByUserInput);
	void HandleParallelAnimation(IScriptedAnimation* animation, const bool& requestedByUserInput);
	const bool ProcessMessage(ParameterContainer& input, ParameterContainer& output);
	const bool SetFunctionPointer(const std::string& functionName, /*std::function<LuaFunctionType>*/ LuaFunctionType* fn);
	void SetScene(Scene* scene);
	const bool MarkForDeletion(const std::string& scriptName);
	const bool LoadScript(const std::string& scriptName, const std::string& scriptSource);	
	// Runs a script, but doesn't save it (originally used to set the ObjectID)
	const bool RunScriptImmediately(const std::string& script);
	// Call all the active scripts that are loaded
	void Update(const FrameTime& deltaTime);
	void Dispose();

	void RegisterInternalLuaFunctions();
	static int MTH_Smoothstep(lua_State* L);
	static int SYS_Printf(lua_State* L);
	static int SYS_GetRunTime(lua_State* L);
	static int SYS_GetDeltaTime(lua_State* L);
	static int SYS_DisposeScript(lua_State* L);
	static int ENT_ApplyForce(lua_State* L);
	static int ENT_SetPosition(lua_State* L);
	static int ENT_GetPosition(lua_State* L);
	static int SYS_TriggerAnimation(lua_State* L);
};