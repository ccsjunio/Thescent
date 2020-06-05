--ObjectFollowObject
--This script will cause an object to follow another

--Constants
--These are set when the GameLuaAnimator loads the script	
local scriptName = UNIQUE_SCRIPT_NAME
local sceneName = SCENE_NAME
local entityName = ENTITY_NAME
local targetPositionX = TARGET_POSITION_X
local targetPositionY = TARGET_POSITION_Y
local targetPositionZ = TARGET_POSITION_Z
local maxSpeed = TOP_SPEED
local maxDistTopSpeed = MAX_DISTANCE_TOPMOVEMENT
local minDistNoSpeed = MIN_DISTANCE_NOMOVEMENT
local startTime = START_TIME

--Variables
local rc, t = SYS_GetRunTime()

if t < startTime then
	do return end --Do not continue if the start time hasn't been reached yet
end

local vX = 0.0
local vY = 0.0
local vZ = 0.0
rc, pX, pY, pZ = ENT_GetPosition(sceneName, entityName)

if rc == 0 then 
	do return end
end

--Distance
local mVecX = targetPositionX - pX
local mVecY = targetPositionY - pY
local mVecZ = targetPositionZ - pZ

--Distance length
local currentDist = math.sqrt(mVecX * mVecX + mVecY * mVecY + mVecZ * mVecZ)

--Direction (normalize mVec)
local normalized_mVecX = mVecX / currentDist
local normalized_mVecY = mVecY / currentDist
local normalized_mVecZ = mVecZ / currentDist

--Velocity
if(currentDist > maxDistTopSpeed) then
	vX = normalized_mVecX * maxSpeed
	vY = normalized_mVecY * maxSpeed
	vZ = normalized_mVecZ * maxSpeed
elseif(currentDist < minDistNoSpeed) then
	vX = normalized_mVecX * 0
	vY = normalized_mVecY * 0
	vZ = normalized_mVecZ * 0
	
	local rc = SYS_DisposeScript(scriptName) --Disposes the script if object has fully moved
	do return end
else 
	local distSpdRatio = 1.0
	local distDiffUpperBound = maxDistTopSpeed - minDistNoSpeed
	local distDiffLowerBound = currentDist - minDistNoSpeed
	
	distSpdRatio = distDiffLowerBound / distDiffUpperBound
	
	local rc, ss = MTH_Smoothstep(0.0, 1.0, distSpdRatio)
	
	vX = normalized_mVecX * ss
	vY = normalized_mVecY * ss
	vZ = normalized_mVecZ * ss
end

--Position
local rc, dt = SYS_GetDeltaTime()
pX = pX + vX * dt
pY = pY + vY * dt
pZ = pZ + vZ * dt

ENT_SetPosition(sceneName, entityName, pX, pY, pZ)