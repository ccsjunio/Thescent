--ApplyForce
--This script will apply a force to a rigid body

--Constants
--These are set when the Scripting System loads the script	
local scriptName = UNIQUE_SCRIPT_NAME
local sceneName = SCENE_NAME
local entityName = ENTITY_NAME
local force = FORCE
local startTime = START_TIME

--Variables
local rc, t = SYS_GetRunTime()

if t < startTime then
	do return end --Do not continue if the start time hasn't been reached yet
end

chunks = {} --Should be a vector3
for substring in force:gmatch("%S+") do
   table.insert(chunks, substring)
end

rc = ENT_ApplyForce(sceneName, entityName, tonumber(chunks[1]), tonumber(chunks[2]), tonumber(chunks[3]))

rc = SYS_DisposeScript(scriptName) --Disposes the script if object has fully moved

do return end
	
