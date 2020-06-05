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

uniform bool offsetAlongNormal;			//When true will cause the vertex to shift by an offset value
uniform float offset;					//If offsetAlongNormal is enabled, then this offset value
uniform mat4 instance_matrices[100];	//Stores all model matrices of the mesh

void ShaderInstancedVertex();

//Program
void main()
{
	ShaderInstancedVertex();
}

/*A basic vertex draw*/
void ShaderInstancedVertex()
{
	/*Takes the model matrix from the instance rendering array of matrices*/
	mat4 instanceModelMatrix = instance_matrices[gl_InstanceID];

	/*Transform the normal in world space*/
	vec3 normal = normalize(vertexNormal.xyz);
	outVertexNormal = inverse(transpose(instanceModelMatrix)) * vec4(normal, 1.0f);
	outVertexNormal.xyz = normalize(outVertexNormal.xyz); 

	/*Resolves the vertex*/
	vec4 vertPosition = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	if(offsetAlongNormal)
	{
		vertPosition = vertexPosition + (outVertexNormal * offset);
	}
	else
	{
		vertPosition = vertexPosition;
	}

	/*Calculates the "camera space" matrix*/
	mat4 mvp = matrixProjection * matrixView * instanceModelMatrix;
    gl_Position = mvp * vec4(vertPosition.xyz, 1.0);

	/*Transforms the vertex in world space*/
	outVertexWorldLocation = instanceModelMatrix * vec4(vertexPosition.xyz, 1.0);		
	
	/*Forwards the color of the vertex and its texture to the next shader.*/
    outVertexColor = vertexColor;	
	outVertexTexture = vertexTexture;	
}