--PrintfTest
--Teleports an object to a position

local scriptName = UNIQUE_SCRIPT_NAME
local sceneName = SCENE_NAME
local startTime = START_TIME
local durationTime = DURATION_TIME
local endTime = startTime + durationTime

local rc, t = SYS_GetRunTime()

if t < startTime then
	do return end --Do not continue if the start time hasn't been reached yet
end

if t > endTime then
    --local err = "Timeout: start=" .. startTime .. " t=" .. t .. " endTime=" .. endTime
	--SYS_Printf(err)
	local rc = SYS_DisposeScript(scriptName) --Once the timespan has passed, the script calls dispose on itself
	do return end
end

local str = "THIS IS A LUA TEST\n"
SYS_Printf(str)

--Example
--<Animation Name="PrintfTest" AnimationType="parallel" AnimationTrigger="immediate" Type="LuaScriptedAnimation">
	--<IsActive>0</IsActive>
	--<Delay>0</Delay>
	--<Scripts>
		--<Script Path="PrintfTest.lua" Type="LuaScript">
			--<Parameters>					
				--<START_TIME Type="float">0</START_TIME>
				--<DURATION_TIME Type="float">5</DURATION_TIME>
				--<SCENE_NAME Type="string">Scene1</SCENE_NAME>
			--</Parameters>			
		--</Script>
	--</Scripts>
--</Animation>
