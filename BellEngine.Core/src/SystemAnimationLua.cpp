#include <SystemAnimationLua.h>
#include <iostream>

Level* SystemAnimationLua::level;

SystemAnimationLua::SystemAnimationLua()
{
	this->level = nullptr;

	InitializeLuaRuntime();
}
SystemAnimationLua::~SystemAnimationLua()
{
	DisposeLuaRuntime();
	ClearScripts();
}

const bool SystemAnimationLua::IsUpdateComplete()
{
	return this->isUpdateComplete;
}
/*Adds a script name to a list of scripts to be deleted on the next frame.*/
bool SystemAnimationLua::MarkForDeletion(std::string scriptName)
{
	bool rc = false;

	std::vector<std::string>::iterator it = std::find(disposeList.begin(), disposeList.end(), scriptName);
	if (it == disposeList.end())
	{
		disposeList.push_back(scriptName);
		rc = true;
	}

	return rc;
}
/*Adds a script to the map of scripts.*/
void SystemAnimationLua::LoadScript(std::string scriptName, std::string scriptSource)
{
	this->scripts[scriptName] = scriptSource;
}
/*Removes a script from the map of scripts.*/
bool SystemAnimationLua::DeleteScript(std::string scriptName)
{
	bool rc = false;

	auto it = scripts.find(scriptName);
	rc = it != scripts.end();

	if (rc)
	{
		this->scripts.erase(it);
	}
	
	return rc;
}
/*Sets a pointer to the current game level.*/
void SystemAnimationLua::SetLevel(Level* level)
{
	this->level = level;
}
/*Runs all the scripts in the map of scripts.*/
void SystemAnimationLua::Update(const FrameTime& deltaTime)
{
	if (!this->IsActive()) return;
	SetFrameTime(deltaTime);

	isUpdateComplete = false;
	
	/*Loops through all scripts.*/
	for(std::map< std::string, std::string>::iterator it = this->scripts.begin(); it != this->scripts.end(); it++ )
	{
		std::string scriptContent = it->second;

		int error = luaL_loadstring(this->luaState, scriptContent.c_str());
		if (error == 0)
		{
			//From LuaBrain
			//Execute function in "protected mode", where problems are 
			//caught and placed on the stack for investigation
			error = lua_pcall(this->luaState,	/* lua state */
											0,	/* nargs: number of arguments pushed onto the lua stack */
											0,	/* nresults: number of results that should be on stack at end*/
											0);	/* errfunc: location, in stack, of error function.
													 if 0, results are on top of stack. */
			if (error != 0 /*no error*/)
			{
				std::cout << "Lua: There was an error..." << std::endl;
				std::cout << this->GetLuaErrorString(error) << std::endl;

				std::string luaError;
				// Get error information from top of stack (-1 is top)
				luaError.append(lua_tostring(this->luaState, -1));

				// Make error message a little more clear
				std::cout << "-------------------------------------------------------" << std::endl;
				std::cout << "Error running Lua script: ";
				std::cout << it->first << std::endl;
				std::cout << luaError << std::endl;
				std::cout << "-------------------------------------------------------" << std::endl;
				// We passed zero (0) as errfunc, so error is on stack)
				lua_pop(this->luaState, 1);  /* pop error message from the stack */
			}
		}
		else
		{
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << it->first << std::endl;
			std::cout << this->GetLuaErrorString(error) << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
		}
	}
	
	//Removes all scripts that have been marked for deletion
	if (disposeList.size() > 0)
	{
		for (unsigned int i = 0; i < disposeList.size(); i++)
		{
			DeleteScript(disposeList[i]);
		}

		disposeList.clear();
		disposeList.shrink_to_fit();
	}	

	isUpdateComplete = true;
}
/*Runs any lua script*/
void SystemAnimationLua::RunScriptImmediately(std::string script)
{
	//From LuaBrain
	//Runs a script, but doesn't save it (originally used to set the ObjectID)
	int error = luaL_loadstring(this->luaState, script.c_str());

	if ( error != 0 /*no error*/)	
	{
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << this->GetLuaErrorString(error) << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		return;
	}

	// execute funtion in "protected mode", where problems are 
	//  caught and placed on the stack for investigation
	error = lua_pcall(this->luaState,	/* lua state */
						0,	/* nargs: number of arguments pushed onto the lua stack */
						0,	/* nresults: number of results that should be on stack at end*/
						0);	/* errfunc: location, in stack, of error function. 
								if 0, results are on top of stack. */
	if ( error != 0 /*no error*/)	
	{
		std::cout << "Lua: There was an error..." << std::endl;
		std::cout << this->GetLuaErrorString(error) << std::endl;

		std::string luaError;
		// Get error information from top of stack (-1 is top)
		luaError.append( lua_tostring(this->luaState, -1) );

		// Make error message a little more clear
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << luaError << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		// We passed zero (0) as errfunc, so error is on stack)
		lua_pop(this->luaState, 1);  /* pop error message from the stack */
	}

	return;
}
/*Initializes Lua.*/
void SystemAnimationLua::InitializeLuaRuntime()
{
	// Create new Lua state.
	// NOTE: this is common to ALL script in this case
	this->luaState = luaL_newstate();

	luaL_openlibs(this->luaState);					/* Lua 5.3.3 */
}
/*Disposes Lua*/
void SystemAnimationLua::DisposeLuaRuntime()
{
	if (luaState)
	{
		lua_close(luaState);
	}
}
/*Returns a Lua error string acconrding to its value.*/
std::string SystemAnimationLua::GetLuaErrorString(int error)
{
	switch ( error )
	{
	case 0:
		return "Lua: no error";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error"; 
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	}//switch ( error )

	// Who knows what this error is?
	return "Lua: UNKNOWN error";
}
/*Removes all scripts in the local map.*/
void SystemAnimationLua::ClearScripts()
{
	scripts.clear();
}
