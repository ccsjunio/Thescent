--TriggerAnimation
--This script triggers an animation defined in level.xml

--Constants
--These are set when the GameLuaAnimator loads the script
local scriptName = UNIQUE_SCRIPT_NAME
local sceneName = SCENE_NAME
local animationName = ANIMATION_NAME

SYS_TriggerAnimation(sceneName, animationName)	--Will call another animation
local rc = SYS_DisposeScript(scriptName) --Once the timespan has passed, the script calls dispose on itself