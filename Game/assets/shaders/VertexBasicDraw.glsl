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

out vec4 outVertexColor;	
out vec4 outVertexWorldLocation;	// Location of the vertex in the world
out vec4 outVertexNormal;
out vec4 outVertexTexture;	

uniform mat4 matrixModel;
uniform mat4 matrixModelInverseTranspose;
uniform mat4 matrixView;
uniform mat4 matrixProjection;

/*Stores information for drawing the vertex*/
struct ShaderBasicVertexInfo_s
{
	bool offsetAlongNormal; 
	float offset;
};
uniform ShaderBasicVertexInfo_s shaderBasicVertexInfo;

void ShaderBasic();

//Program
void main()
{
	ShaderBasic();
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