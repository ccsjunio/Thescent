//Shader Definition
//ShaderType: 2
#version 420

//Camera
uniform vec4 eyeLocation;
uniform vec2 clippingPlaneInfo; //x = near, y = far

//Inputs - from vertex shader, etc.
in vec4 outoutVertexTexture;	
in vec4 outVertexWorldLocation;
in vec4 outVertexNormal;
in vec4 outVertexTexture;	

//Outputs
out vec4 pixelColour;			// RGB A   (0 to 1) 

uniform vec4 specularColor;
uniform vec4 diffuseColour;	
uniform int textureID;
uniform vec2 textureOffsetVec;
uniform int textureIndex = 0;
uniform sampler2D fullscreenTexture;				/*A fullscreen texture used by the buffer passes.*/
uniform samplerCube skybox;							/*The skybox texture.*/
const int MAX_TEXTURE_COUNT = 30;					/*An arbitrary max amount of textures present for the current pixel*/
uniform sampler2D textureSample[MAX_TEXTURE_COUNT];	/*The array of textures for the current pixel*/
uniform int textureCount;							/*The number of textures present for the current pixel*/
uniform int transparency;							/*0 - Alpha, 1 = Discard*/
uniform vec2 uvoffset;								/*An xy offset added to the textRGB*/
uniform vec2 tiling;								/*Used to repeat textures in a surface*/

void ShaderImposter(out vec4 result);

void main()  
{
	vec4 result = vec4(0.0, 0.0, 0.0, 0.0);
	ShaderImposter(result);
	pixelColour.rgb = result.rgb;
	pixelColour.a = result.a;
}

//This shader is used for imposter objects.
void ShaderImposter(out vec4 result)
{
	//Blends all textures by default.
	vec3 texRGB = vec3(0.0, 0.0, 0.0);
	for(int i = 0; i < textureCount; i++)
	{
		vec3 temptexRGB = texture(textureSample[i], outVertexTexture.st * tiling + uvoffset).rgb;
		texRGB += temptexRGB;
	}
	texRGB = texRGB / textureCount;

	result.rgb = texRGB.rgb;
			
	// Note that your eye doesn't see this, 
	// Use this equation instead: 0.21 R + 0.72 G + 0.07 B
	float grey = (texRGB.r + texRGB.g + texRGB.b)/3.0f;
		
	// If it's REALLY black, then discard
	if (grey < 0.05) 	
	{	
		discard; 
	}
		
	// Otherwise control alpha with "black and white" amount
	pixelColour.a = grey;
	if (pixelColour.a < diffuseColour.a)
	{
		result.a = diffuseColour.a;
	}
}