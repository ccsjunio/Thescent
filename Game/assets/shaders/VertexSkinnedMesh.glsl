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

const int MAXNUMBEROFBONES = 100;
uniform mat4 matrixBonesArray[MAXNUMBEROFBONES];

void ShaderSkinnedMesh();

//Program
void main()
{
	ShaderSkinnedMesh();
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