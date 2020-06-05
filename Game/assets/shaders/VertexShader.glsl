//Shader Definition
//ShaderType: 1
#version 420

//Shader fields
layout (location = 1)in vec4 vertexColor;		//Color
layout (location = 2)in vec4 vertexPosition;		//Position
layout (location = 3)in vec4 vertexNormal;		//Normals
layout (location = 4)in vec4 vertexTexture; 		//Textures
layout (location = 5)in vec4 vertexTangent;		//For bump mapping
layout (location = 6)in vec4 vertexBiNormal;		//For bump mapping
layout (location = 7)in vec4 vertexBoneID;		//For skinned mesh (FBX)
layout (location = 8)in vec4 vertexBoneWeight;	//For skinned mesh (FBX)

out vec4 outVertexColor;	
out vec4 outVertexWorldLocation;	// Location of the vertex in the world
out vec4 outVertexNormal;
out vec4 outVertexTexture;	

uniform mat4 matrixModel;
uniform mat4 matrixModelInverseTranspose;
uniform mat4 matrixView;
uniform mat4 matrixProjection;

struct TimeInfo
{
	float deltaTime;
	float runTime;
};
uniform TimeInfo timeInfo;

uniform int vertexShaderType = 0; //0 - Basic, 1 - HeighMap

//HeighMap Shader
uniform sampler2D heightMap;
uniform vec2 texturetOffset;
uniform vec2 swizzle;
uniform float heightMapMRatio;
//-------------------

const int MAXNUMBEROFBONES = 100;
uniform mat4 matrixBonesArray[MAXNUMBEROFBONES];

const int MAX_VERTEXSHADER_FEATURES = 30;
struct VertexShaderInfo_s
{
	int shaderFeatureArrayCount;					/*How many shaders are in use. Ex: 2 shaders (vertex+colorize)*/
	int shaderFeatureArray[MAX_VERTEXSHADER_FEATURES];	/*And array of IDs. Each shader has an ID, which is a function in the uber shader.*/
};
uniform VertexShaderInfo_s vertexShaderInfo;

/*Stores information for drawing the vertex*/
struct ShaderBasicVertexInfo_s
{
	bool offsetAlongNormal; 
	float offset;
};
uniform ShaderBasicVertexInfo_s shaderBasicVertexInfo;

void ShaderBasic();
void ShaderHeightMap();
void ShaderSkinnedMesh();

//Program
void main()
{
	for(int i = 0; i < vertexShaderInfo.shaderFeatureArrayCount; i++)
	{
		/*Apply shader to vertex*/
		switch(vertexShaderInfo.shaderFeatureArray[i])
		{
			case 0: /*Reserved*/ break;
			case 1: ShaderBasic(); break;	
			case 2:	ShaderHeightMap(); break;		
			case 3: ShaderSkinnedMesh(); break;		
			default: break;
		}
	}
}

/*A basic vertex draw*/
void ShaderBasic()
{
	/*Transform the normal in world space*/
	vec3 normal = normalize(vertexNormal.xyz);
	outVertexNormal = inverse(transpose(matrixModel)) * vec4(normal, 1.0f);
	outVertexNormal.xyz = normalize(outVertexNormal.xyz); 

	/*Resolves the vertex*/
	vec4 vertPosition = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	if(shaderBasicVertexInfo.offsetAlongNormal)
	{
		vertPosition = vertexPosition + (outVertexNormal * shaderBasicVertexInfo.offset);
	}
	else
	{
		vertPosition = vertexPosition;
	}

	/*Calculates the "camera space" matrix*/
	mat4 mvp = matrixProjection * matrixView * matrixModel;
    gl_Position = mvp * vec4(vertPosition.xyz, 1.0);

	/*Transforms the vertex in world space*/
	outVertexWorldLocation = matrixModel * vec4(vertexPosition.xyz, 1.0);		
	
	/*Forwards the color of the vertex and its texture to the next shader.*/
    outVertexColor = vertexColor;	
	outVertexTexture = vertexTexture;	
}

void ShaderHeightMap()
{
	vec4 vertPosition = vertexPosition;



	// Move the y value by some amount from texture	
	// Since it's black and white, I only sample 1 colour.		
	vec2 texUV1 = vertexTexture.st + texturetOffset.xy;
	// This will pick a completely different location
	// (note the reversal of the xy to yx, called a "swizzle")
	vec2 texUV2 = vertexTexture.st + texturetOffset.yx * swizzle;		
	float texValue1 = texture(heightMap, texUV1.st).r;
	float texValue2 = texture(heightMap, texUV2.st).r;	
	float texValue = (texValue1 + texValue2) * heightMapMRatio;		
	vertPosition.y += (texValue * heightMapMRatio);



	mat4 mvp = matrixProjection * matrixView * matrixModel;
    gl_Position = mvp * vec4(vertPosition.xyz, 1.0);

	// Vertex location in "world space"
	outVertexWorldLocation = matrixModel * vec4(vertPosition.xyz, 1.0);		
	
 	outVertexNormal = vertexNormal;
	
	// Pass the colour and UV unchanged.
    outVertexColor = vertexColor;	
	outVertexTexture = vertexTexture;	
}

void ShaderSkinnedMesh()
{
	// *************************************************
	mat4 BoneTransform = matrixBonesArray[ int(vertexBoneID[0]) ] * vertexBoneWeight[0];
	BoneTransform += matrixBonesArray[ int(vertexBoneID[1]) ] * vertexBoneWeight[1];
	BoneTransform += matrixBonesArray[ int(vertexBoneID[2]) ] * vertexBoneWeight[2];
	BoneTransform += matrixBonesArray[ int(vertexBoneID[3]) ] * vertexBoneWeight[3];

	// Apply the bone transform to the vertex:
	vec4 vertOriginal = vec4(vertexPosition.xyz, 1.0f);
	vec4 vertAfterBoneTransform = BoneTransform * vertOriginal;
				
	mat4 matMVP =  matrixProjection * matrixView * matrixModel;	
	// Transform the updated vertex location (from the bone)
	//  and transform with model view projection matrix (as usual)
	gl_Position = matMVP * vertAfterBoneTransform;
				
	// Then we do the normals, etc.
	outVertexWorldLocation = matrixModel * vertAfterBoneTransform;	
		
	// Updated "world" or "model" transform 
	mat4 matModelAndBone = matrixModel * BoneTransform;
		
	vec3 theNormal = normalize(vertexNormal.xyz);
	outVertexNormal = inverse(transpose(matModelAndBone)) * vec4(theNormal, 1.0f);
	outVertexNormal.xyz = normalize(outVertexNormal.xyz); 
		
	// Pass the colour and UV unchanged.
    outVertexColor = vertexColor;	
	outVertexTexture = vertexTexture;	
}