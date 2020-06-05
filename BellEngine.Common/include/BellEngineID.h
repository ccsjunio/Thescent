#pragma once

//ID OFFSETS

static const unsigned long long ENGINE_OBJ_SCENERY_OFFSET					= 1000ull;
static const unsigned long long ENGINE_OBJ_PHYSICS_OFFSET					= 3000ull;
static const unsigned long long ENGINE_OBJ_GRAPHICS_OFFSET					= 4000ull;
static const unsigned long long ENGINE_OBJ_AI_OFFSET						= 5000ull;
static const unsigned long long ENGINE_OBJ_SOUND_OFFSET						= 6000ull;
static const unsigned long long ENGINE_OBJ_MESH_OFFSET						= 7000ull;
static const unsigned long long ENGINE_OBJ_SCRIPT_OFFSET					= 8000ull;
static const unsigned long long ENGINE_OBJ_SHADER_OFFSET					= ENGINE_OBJ_GRAPHICS_OFFSET + 200ull;

//SCENERY IDS

static const unsigned long long ENGINE_OBJ_ID_SCN_GAMEOBJECT				= ENGINE_OBJ_SCENERY_OFFSET + 1ull;
static const unsigned long long ENGINE_OBJ_ID_SCN_SKYBOX					= ENGINE_OBJ_SCENERY_OFFSET + 2ull;
static const unsigned long long ENGINE_OBJ_ID_SCN_CLOTH						= ENGINE_OBJ_SCENERY_OFFSET + 3ull;
static const unsigned long long ENGINE_OBJ_ID_SCN_LEVEL						= ENGINE_OBJ_SCENERY_OFFSET + 4ull;
static const unsigned long long ENGINE_OBJ_ID_SCN_SCENE						= ENGINE_OBJ_SCENERY_OFFSET + 5ull;
static const unsigned long long ENGINE_OBJ_ID_SCN_SCRIPT					= ENGINE_OBJ_SCENERY_OFFSET + 6ull;

//GRAPHICS IDS

static const unsigned long long ENGINE_OBJ_ID_GPH_FBO						= ENGINE_OBJ_GRAPHICS_OFFSET + 1ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_STENCIL					= ENGINE_OBJ_GRAPHICS_OFFSET + 2ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_LIGHTPOINT				= ENGINE_OBJ_GRAPHICS_OFFSET + 3ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_LIGHTDIRECTIONAL			= ENGINE_OBJ_GRAPHICS_OFFSET + 4ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_LIGHTSPOT					= ENGINE_OBJ_GRAPHICS_OFFSET + 5ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_CAMERAFREE				= ENGINE_OBJ_GRAPHICS_OFFSET + 6ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_CAMERALOCKEDAT			= ENGINE_OBJ_GRAPHICS_OFFSET + 7ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_SHADER					= ENGINE_OBJ_GRAPHICS_OFFSET + 8ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_SHADERPROGRAM				= ENGINE_OBJ_GRAPHICS_OFFSET + 9ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_TEXTURE2D					= ENGINE_OBJ_GRAPHICS_OFFSET + 10ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_TEXTURESKYBOX				= ENGINE_OBJ_GRAPHICS_OFFSET + 11ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_TEXTUREHEIGHMAP			= ENGINE_OBJ_GRAPHICS_OFFSET + 12ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_RENDERCOMPONENT			= ENGINE_OBJ_GRAPHICS_OFFSET + 13ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_CLOTHRENDERCOMPONENT		= ENGINE_OBJ_GRAPHICS_OFFSET + 14ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_MESHCOMPONENT				= ENGINE_OBJ_GRAPHICS_OFFSET + 15ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_SKINNEDMESHCOMPONENT		= ENGINE_OBJ_GRAPHICS_OFFSET + 16ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_PARTICLEEMITTERCOMPONENT	= ENGINE_OBJ_GRAPHICS_OFFSET + 17ull;

static const unsigned long long ENGINE_OBJ_ID_GPH_GLSHADER_VERTEX_BASIC					= ENGINE_OBJ_SHADER_OFFSET + 18ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_GLSHADER_VERTEXINSTANCE_0				= ENGINE_OBJ_SHADER_OFFSET + 19ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_GLSHADER_VERTEX_SKINNED				= ENGINE_OBJ_SHADER_OFFSET + 20ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_GLSHADER_FRAGMENT_COLORIZE			= ENGINE_OBJ_SHADER_OFFSET + 21ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_GLSHADER_FRAGMENT_TEXTURIZE			= ENGINE_OBJ_SHADER_OFFSET + 22ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_GLSHADER_FRAGMENT_TEXTURIZE_SKYBOX	= ENGINE_OBJ_SHADER_OFFSET + 23ull;
static const unsigned long long ENGINE_OBJ_ID_GPH_GLSHADER_FRAGMENT_IMPOSTER			= ENGINE_OBJ_SHADER_OFFSET + 24ull;

//PHYSICS IDS

static const unsigned long long ENGINE_OBJ_ID_PHY_RIGIDBODY					= ENGINE_OBJ_PHYSICS_OFFSET + 1ull;
static const unsigned long long ENGINE_OBJ_ID_PHY_SOFTBODY					= ENGINE_OBJ_PHYSICS_OFFSET + 2ull;
static const unsigned long long ENGINE_OBJ_ID_PHY_SOFTBODYNODE				= ENGINE_OBJ_PHYSICS_OFFSET + 3ull;
static const unsigned long long ENGINE_OBJ_ID_PHY_COLLIDERAABB				= ENGINE_OBJ_PHYSICS_OFFSET + 4ull;
static const unsigned long long ENGINE_OBJ_ID_PHY_COLLIDERSPHERE			= ENGINE_OBJ_PHYSICS_OFFSET + 5ull;
static const unsigned long long ENGINE_OBJ_ID_PHY_COLLIDERPLANE				= ENGINE_OBJ_PHYSICS_OFFSET + 6ull;
static const unsigned long long ENGINE_OBJ_ID_PHY_COLLIDERBOX				= ENGINE_OBJ_PHYSICS_OFFSET + 7ull;
static const unsigned long long ENGINE_OBJ_ID_PHY_COLLIDERCAPSULE			= ENGINE_OBJ_PHYSICS_OFFSET + 8ull;

//SOUND IDS

static const unsigned long long ENGINE_OBJ_ID_SND_SOUND						= ENGINE_OBJ_SOUND_OFFSET + 1ull;
static const unsigned long long ENGINE_OBJ_ID_SND_SOUNDCOMPONENT			= ENGINE_OBJ_SOUND_OFFSET + 2ull;

//AI IDS

static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORSEEK				= ENGINE_OBJ_AI_OFFSET + 1ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORFLEE				= ENGINE_OBJ_AI_OFFSET + 2ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORPURSUE				= ENGINE_OBJ_AI_OFFSET + 3ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIOREVADE				= ENGINE_OBJ_AI_OFFSET + 4ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORWANDER				= ENGINE_OBJ_AI_OFFSET + 5ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORLOOKATTARGET		= ENGINE_OBJ_AI_OFFSET + 6ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORLOOKATMOVINGDIR	= ENGINE_OBJ_AI_OFFSET + 7ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORFLOCKING			= ENGINE_OBJ_AI_OFFSET + 8ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORPATHFOLLOWING		= ENGINE_OBJ_AI_OFFSET + 9ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORFORMATION			= ENGINE_OBJ_AI_OFFSET + 10ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORBFS				= ENGINE_OBJ_AI_OFFSET + 11ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORDFS				= ENGINE_OBJ_AI_OFFSET + 12ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORDJIKSTRA			= ENGINE_OBJ_AI_OFFSET + 13ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORASTAR				= ENGINE_OBJ_AI_OFFSET + 14ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIORSHOOTAT			= ENGINE_OBJ_AI_OFFSET + 15ull;
static const unsigned long long ENGINE_OBJ_ID_AI_BEHAVIOREVADEBYMATCH		= ENGINE_OBJ_AI_OFFSET + 16ull;

//MESH IDS

static const unsigned long long ENGINE_OBJ_ID_MSH_MESH						= ENGINE_OBJ_MESH_OFFSET + 1ull;
static const unsigned long long ENGINE_OBJ_ID_MSH_MESHCOMPONENT				= ENGINE_OBJ_MESH_OFFSET + 2ull;
static const unsigned long long ENGINE_OBJ_ID_MSH_PARTICLECOMPONENT			= ENGINE_OBJ_MESH_OFFSET + 3ull;

//SCRIPTING IDS

static const unsigned long long ENGINE_OBJ_ID_SCP_ANIMATION					= ENGINE_OBJ_SCRIPT_OFFSET + 1ull;
static const unsigned long long ENGINE_OBJ_ID_SCP_LUANIMATION				= ENGINE_OBJ_SCRIPT_OFFSET + 2ull;
static const unsigned long long ENGINE_OBJ_ID_SCP_LUASCRIPT					= ENGINE_OBJ_SCRIPT_OFFSET + 3ull;