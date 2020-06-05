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
#include <Level.h>
#include <ISystem.h>

class SystemAnimationLua : public ISystem
{
private:
	std::vector<std::string> disposeList;
	bool isUpdateComplete;

	void InitializeLuaRuntime();
	void DisposeLuaRuntime();
	bool DeleteScript(std::string scriptName);

protected:
	std::map< std::string /*scriptName*/, std::string /*scriptSource*/ > scripts;
	lua_State* luaState;
	static Level* level;
	
	SystemAnimationLua();
	
	std::string GetLuaErrorString(int error);
	void ClearScripts();

public:	
	~SystemAnimationLua();

	const bool IsUpdateComplete();
	bool MarkForDeletion(std::string scriptName);
	void LoadScript(std::string scriptName, std::string scriptSource);	
	void SetLevel(Level* level);
	virtual void RegisterFunctions() = 0;	
	// Runs a script, but doesn't save it (originally used to set the ObjectID)
	void RunScriptImmediately(std::string script);
	// Call all the active scripts that are loaded
	void Update(const FrameTime& deltaTime);
};