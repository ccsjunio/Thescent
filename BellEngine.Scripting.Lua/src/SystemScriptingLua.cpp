#include <MathLibrary.h>
#include <GLCommon.h>
#include <SystemScriptingLua.h>
#include <iostream>
#include <thread>
#include "ScriptedAnimationFactory.h"
#include "LuaScriptFactory.h"
#include <IRigidbody.h>
#include <BellEngineID.h>
#include <LuaScriptedAnimation.h>

std::map<std::string, LuaWorldInfo> SystemScriptingLua::luaWorldInfos;
std::map<std::string, LuaFunctionType*> SystemScriptingLua::functionPointers;
SystemScriptingLua* SystemScriptingLua::scriptingSystem = nullptr;

SystemScriptingLua::SystemScriptingLua()
{
	scriptingSystem = this;
	this->isUpdateComplete.store(false);
	this->isActive.store(true);
	this->scene = nullptr;
	this->scriptedAnimationFactory = new ScriptedAnimationFactory();
	this->scriptFactory = new LuaScriptFactory();
	RegisterInternalLuaFunctions();
}
SystemScriptingLua::~SystemScriptingLua()
{
	Dispose();
}

/*Plays a scripted animation.*/
const bool SystemScriptingLua::PlayAnimation(const std::string& name, const std::string& sceneName, const bool& requestedByUserInput)
{
	auto itLuaWorldInfo = luaWorldInfos.find(sceneName);
	if (itLuaWorldInfo != luaWorldInfos.end())
	{
		Scene* _scene = (*itLuaWorldInfo).second.scene;

		IScriptedAnimation* animation = _scene->FindAnimationByName(name);
		if (animation && animation->GetTypeID() == ENGINE_OBJ_ID_SCP_LUANIMATION)
		{
			LuaScriptedAnimation* luaAnimation = dynamic_cast<LuaScriptedAnimation*>(animation);
			if (luaAnimation)
			{
				/*This will only fire the animations that are immediate.*/
				if (luaAnimation->GetTriggerID() == (unsigned int)LuaScriptedAnimation::AnimationTrigger_e::Immediate)
				{
					switch (luaAnimation->GetAnimationTypeID())
					{
					case (unsigned int)LuaScriptedAnimation::AnimationType_e::Parallel:
					{
						HandleParallelAnimation(animation, requestedByUserInput);
					}
					return true;
					case (unsigned int)LuaScriptedAnimation::AnimationType_e::Serial:
					{
						std::thread t(&SystemScriptingLua::HandleSerialAnimation, this, animation, requestedByUserInput);
						t.detach();				
					}
					return true;
					default:
						break;
					}
				}
			}
		}
		else
		{
			printf("Could not find animation %s.", name.c_str());
		}
	}

	return false;
}
/*Checks if the update loop has completed.*/
const bool SystemScriptingLua::IsUpdateComplete()
{
	return this->isUpdateComplete.load();
}

/*Returns the scripted animation factory.*/
IFactory<IScriptedAnimation>* SystemScriptingLua::GetScriptedAnimationFactory()
{
	return scriptedAnimationFactory;
}
/*Returns the script factory.*/
IFactory<IScript>* SystemScriptingLua::GetScriptFactory()
{
	return scriptFactory;
}
/*Checks if the system is online or offline.*/
const bool SystemScriptingLua::IsActive()
{
	return this->isActive.load();
}
/*Toggles the system on/off.*/
void SystemScriptingLua::Activate(const bool& value)
{
	this->isActive.store(value);
}
/*Runs a function inside this class.*/
const bool SystemScriptingLua::ProcessMessage(ParameterContainer& input, ParameterContainer& output)
{
	Parameter p;
	bool rc = input.FindParameterByName("command", p);
	if (rc)
	{
		if (p.value == "SetFunctionPointer")
		{
			rc = input.FindParameterByName("functionName", p);
			if (!rc) return false;
			std::string functionName = p.value;

			rc = input.FindParameterByName("fn", p);
			if (!rc) return false;
			//Badass move right here
			LuaFunctionType* fn = (LuaFunctionType*)p.valuePtr;

			//std::function<LuaFunctionType> fn{reinterpret_cast<LuaFunctionType*>(p.valuePtr) };

			return SetFunctionPointer(functionName, fn);
		}
		else if (p.value == "InitializeLuaRuntime")
		{
			InitializeLuaRuntime();
			return true;
		}
		else if (p.value == "MarkForDeletion")
		{
			rc = input.FindParameterByName("scriptName", p);
			if (rc)
			{
				return MarkForDeletion(p.value);
			}
			else
			{
				return false;
			}		
		}

		return false;
	}
	else
	{
		return false;
	}
}
/*REgisters the static function being called by lua.*/
const bool SystemScriptingLua::SetFunctionPointer(const std::string& functionName, /*std::function<LuaFunctionType>*/ LuaFunctionType* fn)
{
	auto it = functionPointers.find(functionName);
	if (it == functionPointers.end())
	{
		//auto f = std::bind(fn, std::placeholders::_1);
		
		functionPointers[functionName] = fn;
		return true;
	}

	return false;
}
/*Sets the current scene*/
void SystemScriptingLua::SetScene(Scene* scene)
{
	this->scene = scene;

	if (luaWorldInfos.find(this->scene->GetName()) == luaWorldInfos.end())
	{
		InitializeLuaRuntime();
	}
}
/*Initializes Lua.*/
void SystemScriptingLua::InitializeLuaRuntime()
{
	//typedef void function_t(lua_State*);

	LuaWorldInfo lwi;
	lwi.scene = scene;
	lwi.luaState = luaL_newstate();

	luaL_openlibs(lwi.luaState);					/* Lua 5.3.3 */
	
	//Sets static function pointers used by the scripts
	for (std::pair<std::string, LuaFunctionType* /*std::function<void(lua_State*)>*/> element : functionPointers)
	{
		lua_pushcfunction(lwi.luaState, (lua_CFunction)element.second);
		lua_setglobal(lwi.luaState, element.first.c_str());

		/*auto ptr_fun = element.second.target<LuaFunctionType>();
		if (ptr_fun)
		{
			lua_pushcfunction(lwi.luaState, (lua_CFunction)ptr_fun);
			lua_setglobal(lwi.luaState, element.first.c_str());
		}*/
	}

	luaWorldInfos[scene->GetName()] = lwi;
}
/*Adds a script name to a list of scripts to be deleted on the next frame.*/
const bool SystemScriptingLua::MarkForDeletion(const std::string& scriptName)
{
	auto itLWI = luaWorldInfos.find(scene->GetName());
	if (itLWI == luaWorldInfos.end())
	{
		return false;
	}

	std::vector<std::string>::iterator it = std::find(itLWI->second.disposeList.begin(), itLWI->second.disposeList.end(), scriptName);
	if (it == itLWI->second.disposeList.end())
	{
		itLWI->second.disposeList.push_back(scriptName);

		return true;
	}
	else
	{
		return false;
	}
}
/*Adds a script to the map of scripts.*/
const bool SystemScriptingLua::LoadScript(const std::string& scriptName, const std::string& scriptSource)
{
	auto it = luaWorldInfos.find(scene->GetName());
	if (it != luaWorldInfos.end())
	{
		it->second.scripts.push_back(new ScriptKVP(scriptName, scriptSource));
		return true;
	}

	return false;
}
/*Removes a script from the map of scripts.*/
const bool SystemScriptingLua::DeleteScript(const std::string& scriptName)
{
	auto itLWI = luaWorldInfos.find(scene->GetName());
	if (itLWI == luaWorldInfos.end())
	{
		return false;
	}

	auto itScript = std::find_if(itLWI->second.scripts.begin(), itLWI->second.scripts.end(), [scriptName](auto& script) { return script->GetName() == scriptName; });
	if (itScript == itLWI->second.scripts.end())
	{
		return false;
	}

	itLWI->second.scripts.erase(itScript);
	
	ScriptKVP* ptr = *itScript;
	if (ptr)
	{
		delete (ptr);
	}

	return true;
}
/*Runs any lua script*/
const bool SystemScriptingLua::RunScriptImmediately(const std::string& script)
{
	auto it = luaWorldInfos.find(scene->GetName());
	if (it == luaWorldInfos.end())
	{
		return false;
	}

	lua_State* luaState = it->second.luaState;

	//From LuaBrain
	//Runs a script, but doesn't save it (originally used to set the ObjectID)
	int error = luaL_loadstring(luaState, script.c_str());

	if ( error != 0 /*no error*/)	
	{
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << this->GetLuaErrorString(error) << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		return false;
	}

	// execute funtion in "protected mode", where problems are 
	//  caught and placed on the stack for investigation
	error = lua_pcall(luaState,	/* lua state */
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
		luaError.append(lua_tostring(luaState, -1));

		// Make error message a little more clear
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << luaError << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		// We passed zero (0) as errfunc, so error is on stack)
		lua_pop(luaState, 1);  /* pop error message from the stack */
	}

	return true;
}
/*Disposes Lua*/
void SystemScriptingLua::DisposeLuaRuntime()
{
	for (std::pair<std::string, LuaWorldInfo> element : luaWorldInfos)
	{
		if (element.second.luaState)
		{
			lua_close(element.second.luaState);
		}

		DisposeVector(element.second.scripts);
	}

	luaWorldInfos.clear();
}
/*Returns a Lua error string acconrding to its value.*/
const std::string SystemScriptingLua::GetLuaErrorString(const int& error)
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
/*Processes a parallelel animation.*/
void SystemScriptingLua::HandleParallelAnimation(IScriptedAnimation* animation, const bool& requestedByUserInput)
{
	std::vector<IScript*> scripts;
	if (animation->GetScripts(scripts))
	{
		for (const auto& script : scripts)
		{
			//If the animation has a delay
			if (animation->GetDelay() != 0.0f)
			{
				ParameterContainer params;
				if (script->GetParameters(params))
				{
					//Overrides START_TIME of the script to add the delay
					Parameter p;
					if (params.FindParameterByName("START_TIME", p))
					{
						float startTime = strtof(p.value.c_str(), NULL);
						float delayedStartTime = startTime + animation->GetDelay();

						//If the animations was called by an user input then adds the current time to start time.
						if (requestedByUserInput)
						{
							delayedStartTime += this->frameTime.GetRunTime();
						}

						params.SetValue("START_TIME", std::to_string(delayedStartTime));
						script->SetParameters(params);
					}
				}
			}
			else
			{
				if (requestedByUserInput)
				{
					ParameterContainer params;
					if (script->GetParameters(params))
					{
						//Overrides START_TIME of the script to add the delay
						Parameter p;
						if (params.FindParameterByName("START_TIME", p))
						{
							float startTime = strtof(p.value.c_str(), NULL);
							float delayedStartTime = startTime + this->frameTime.GetRunTime();

							params.SetValue("START_TIME", std::to_string(delayedStartTime));
							script->SetParameters(params);
						}
					}
				}
			}

			LoadScript(script->GetName(), script->GetContent());
		}
	}
}
/*Processes a serial animation.*/
void SystemScriptingLua::HandleSerialAnimation(IScriptedAnimation* animation, const bool& requestedByUserInput)
{
	std::vector<IScript*> scripts;
	if (animation->GetScripts(scripts))
	{
		//If there is a delay of time... 		
		if (animation->GetDelay() != 0.0f)
		{
			//Wait before pushing the scripts to the LuaAnimator 
			std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 * animation->GetDelay())));
		}

		for(const auto& script : scripts)
		{
			ParameterContainer params;
			if (script->GetParameters(params))
			{
				//Overrides START_TIME of the script to make sure it will start after the previous script
				//Instead of using the START_TIME set in the XML, we are replacing that by the current time in seconds
				Parameter p;
				if (params.FindParameterByName("START_TIME", p))
				{
					params.SetValue("START_TIME", std::to_string(this->frameTime.GetRunTime()));
					script->SetParameters(params);
				}
			}

			LoadScript(script->GetName(), script->GetContent());
			//Hold
			std::string scriptName = script->GetName();
			while (std::find_if(luaWorldInfos[scene->GetName()].scripts.begin(), luaWorldInfos[scene->GetName()].scripts.end(), [scriptName](ScriptKVP*& scp) { return scp->GetName() == scriptName; }) != luaWorldInfos[scene->GetName()].scripts.end())
			{
				//wait until the script has unsubscribed itself from the script map
				//Scripts unsubscribe themselves by calling l_DisposeScript in Lua
			}
		}
	}
}
/*Runs all the scripts in the map of scripts.*/
void SystemScriptingLua::Update(const FrameTime& frameTime)
{
	if (!this->isActive.load()) return;
	this->isUpdateComplete.store(false);

	SetFrameTime(frameTime);

	for (auto& luaWorld : luaWorldInfos)
	{
		/*Loops through all scripts.*/
		for (auto& scriptSequence : luaWorld.second.scripts)
		{
			std::string scriptContent = scriptSequence->GetContent();

			int error = luaL_loadstring(luaWorld.second.luaState, scriptContent.c_str());
			if (error == 0)
			{
				//From LuaBrain
				//Execute function in "protected mode", where problems are 
				//caught and placed on the stack for investigation
				error = lua_pcall(luaWorld.second.luaState,	/* lua state */
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
					luaError.append(lua_tostring(luaWorld.second.luaState, -1));

					// Make error message a little more clear
					printf("-------------------------------------------------------\n");
					printf("Error running Lua script: %s\n", scriptSequence->GetName().c_str());
					printf("Error message: %s\n", luaError.c_str());
					printf("-------------------------------------------------------\n");
					// We passed zero (0) as errfunc, so error is on stack)
					lua_pop(luaWorld.second.luaState, 1);  /* pop error message from the stack */
				}
			}
			else
			{
				printf("-------------------------------------------------------\n");
				printf("Error running Lua script: %s\n", scriptSequence->GetName().c_str());
				printf("Error message: %s\n", this->GetLuaErrorString(error).c_str());
				printf("-------------------------------------------------------\n");
			}
		}

		//Removes all scripts that have been marked for deletion
		size_t disposeListSZ = luaWorld.second.disposeList.size();
		if (disposeListSZ > 0)
		{
			for (size_t i = 0; i < disposeListSZ; i++)
			{
				DeleteScript(luaWorld.second.disposeList[i]);
			}

			luaWorld.second.disposeList.clear();
			luaWorld.second.disposeList.shrink_to_fit();
		}
	}

	this->isUpdateComplete.store(true);
}
/*Clear resources*/
void SystemScriptingLua::Dispose()
{
	DisposeLuaRuntime();
	DisposePointer(scriptFactory);
	DisposePointer(scriptedAnimationFactory);
}
/*Register lua functions inside this scripting system.*/
void SystemScriptingLua::RegisterInternalLuaFunctions()
{
	functionPointers["SYS_Printf"] = SystemScriptingLua::SYS_Printf;
	functionPointers["SYS_GetRunTime"] = SystemScriptingLua::SYS_GetRunTime;
	functionPointers["SYS_GetDeltaTime"] = SystemScriptingLua::SYS_GetDeltaTime;
	functionPointers["SYS_DisposeScript"] = SystemScriptingLua::SYS_DisposeScript;
	functionPointers["ENT_ApplyForce"] = SystemScriptingLua::ENT_ApplyForce;
	functionPointers["ENT_SetPosition"] = SystemScriptingLua::ENT_SetPosition;
	functionPointers["ENT_GetPosition"] = SystemScriptingLua::ENT_GetPosition;
	functionPointers["MTH_Smoothstep"] = SystemScriptingLua::MTH_Smoothstep;
}
/*Provides access to GLM's smoothstep.*/
int SystemScriptingLua::MTH_Smoothstep(lua_State* L)
{
	lua_pushboolean(L, true);

	float edge0 = (float)lua_tonumber(L, 1);
	float edge1 = (float)lua_tonumber(L, 2);
	float ratio = (float)lua_tonumber(L, 3);

	float ss = glm::smoothstep(edge0, edge1, ratio);
	lua_pushnumber(L, ss);

	return 2;
}
/*Prints a message in the console*/
int SystemScriptingLua::SYS_Printf(lua_State* L)
{
	std::string str = (std::string)lua_tostring(L, 1);
	printf(str.c_str());

	lua_pushboolean(L, true);
	return 1;
}
/*Gets the last runTime.*/
int SystemScriptingLua::SYS_GetRunTime(lua_State* L)
{
	lua_pushboolean(L, true);

	lua_pushnumber(L, scriptingSystem->frameTime.GetRunTime());

	return 2;
}
/*Gets the last deltatime*/
int SystemScriptingLua::SYS_GetDeltaTime(lua_State* L)
{
	lua_pushboolean(L, true);

	lua_pushnumber(L, scriptingSystem->frameTime.GetDeltaTime());

	return 2;
}
/*Allows lua script to remove itself from the list of running scripts.*/
int SystemScriptingLua::SYS_DisposeScript(lua_State* L)
{
	std::string scriptName = (std::string)lua_tostring(L, 1);

	lua_pushboolean(L, scriptingSystem->MarkForDeletion(scriptName));
	return 1;
}
int SystemScriptingLua::ENT_ApplyForce(lua_State* L)
{
	std::string sceneName = (std::string)lua_tostring(L, 1);
	std::string entityName = (std::string)lua_tostring(L, 2);

	auto itScene = luaWorldInfos.find(sceneName);

	if (itScene != luaWorldInfos.end())
	{
		LuaWorldInfo info = (*itScene).second;

		Entity* entity = info.scene->FindEntityByName(entityName);
		if (entity)
		{
			IRigidbody* rb = entity->GetComponent<IRigidbody*>();
			if (rb)
			{
				Vector3D force = Vector3D();

				force.x = (float)lua_tonumber(L, 3);
				force.y = (float)lua_tonumber(L, 4);
				force.z = (float)lua_tonumber(L, 5);

				rb->ApplyForce(force);

				lua_pushboolean(L, true);
				return 1;
			}
		}
	}

	lua_pushboolean(L, false);
	return 1;
}
/*Allows lua to call an animation.*/
int SystemScriptingLua::SYS_TriggerAnimation(lua_State* L)
{
	bool rc = false;
	std::string sceneName = (std::string)lua_tostring(L, 1);
	std::string animationName = (std::string)lua_tostring(L, 2);

	auto itScene = luaWorldInfos.find(sceneName);

	if (itScene != luaWorldInfos.end())
	{
		LuaWorldInfo info = (*itScene).second;

		IScriptedAnimation* animation = info.scene->FindAnimationByName(animationName);

		if (animation)
		{
			switch ((LuaScriptedAnimation::AnimationType_e)((LuaScriptedAnimation*)animation)->GetAnimationTypeID())
			{
			case LuaScriptedAnimation::AnimationType_e::Parallel:
			{
				scriptingSystem->HandleParallelAnimation(animation, false);
				rc = true;
			}
			break;
			case LuaScriptedAnimation::AnimationType_e::Serial:
			{
				std::thread t(&SystemScriptingLua::HandleSerialAnimation, scriptingSystem, animation, false);	t.detach();
				rc = true;
			}
			break;
			default:
			{
				rc = false;
			}
			break;
			}
		}
		else
		{
			rc = false;
		}
	}
	

	lua_pushboolean(L, rc);

	return 1;
}
/*Sets the position of an entity*/
int SystemScriptingLua::ENT_SetPosition(lua_State* L)
{
	std::string sceneName = (std::string)lua_tostring(L, 1);
	std::string entityName = (std::string)lua_tostring(L, 2);

	auto itScene = luaWorldInfos.find(sceneName);

	if (itScene != luaWorldInfos.end())
	{
		LuaWorldInfo info = (*itScene).second;

		Entity* entity = info.scene->FindEntityByName(entityName);
		if (entity)
		{
			IRigidbody* rb = entity->GetComponent<IRigidbody*>();
			if (rb)
			{
				Vector3D p = Vector3D();

				p.x = (float)lua_tonumber(L, 3);
				p.y = (float)lua_tonumber(L, 4);
				p.z = (float)lua_tonumber(L, 5);

				rb->SetPosition(p);

				lua_pushboolean(L, true);
				return 1;
			}
		}
	}

	lua_pushboolean(L, false);
	return 1;
}
/*Gets the position of an entity*/
int SystemScriptingLua::ENT_GetPosition(lua_State* L)
{
	std::string sceneName = (std::string)lua_tostring(L, 1);
	std::string entityName = (std::string)lua_tostring(L, 2);

	auto itScene = luaWorldInfos.find(sceneName);

	if (itScene != luaWorldInfos.end())
	{
		LuaWorldInfo info = (*itScene).second;

		Entity* entity = info.scene->FindEntityByName(entityName);
		if (entity)
		{
			IRigidbody* rb = entity->GetComponent<IRigidbody*>();
			if (rb)
			{
				lua_pushboolean(L, true);

				Vector3D p = rb->GetPosition();
				lua_pushnumber(L, p.x);
				lua_pushnumber(L, p.y);
				lua_pushnumber(L, p.z);
								
				return 4;
			}	
		}
	}

	lua_pushboolean(L, false);
	return 1;
}
