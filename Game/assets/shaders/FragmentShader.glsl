//Shader Definition
//ShaderType: 2
#version 420

//World info
uniform vec2 screenResolution;
struct TimeInfo
{
	float deltaTime;
	float runTime;
};
uniform TimeInfo timeInfo;

struct InterpolatinColorize_s
{
	vec4 colorA;
	vec4 colorB;
};
uniform InterpolatinColorize_s interpolatinColorize;

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

//Color information
uniform vec4 specularColor;
uniform vec4 diffuseColour;	
struct Colorize_s
{
	bool isLit;
	vec4 specularColor;
	vec4 diffuseColour;	
};
uniform Colorize_s colorizeInfo;

//Texture information
uniform int textureID;
uniform vec2 textureOffsetVec;
uniform int textureIndex = 0;
const int MAX_TEXTURE_COUNT = 30;	/*An arbitrary max amount of textures present for the current pixel*/
struct TextureProcessInfo
{
	sampler2D fullscreenTexture;				/*A fullscreen texture used by the buffer passes.*/
	samplerCube skybox;							/*The skybox texture.*/
	sampler2D textureSample[MAX_TEXTURE_COUNT];	/*The array of textures for the current pixel*/
	int textureCount;							/*The number of textures present for the current pixel*/
	int transparency;							/*0 - Alpha, 1 = Discard*/
	vec2 uvoffset;								/*An xy offset added to the textRGB*/
	vec2 tiling;								/*Used to repeat textures in a surface*/
};
uniform TextureProcessInfo textureProcessInfo;

//Light information
struct sLight
{
	vec4 position;	//position of the light		
	vec4 diffuse;	//"Solid" color of the light
	vec4 specular;	//rgb = highlight colour, w = power
	vec4 atten;		//x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	//For spot and directional lights
	vec4 param1;	//x = lightType, y = inner angle, z = outer angle, w = TBD
	                //0 = pointlight
					//1 = spot light
					//2 = directional light
	vec4 param2;	//x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBEROFLIGHTS = 30;
uniform sLight lightArray[NUMBEROFLIGHTS];  	// 80 uniforms

vec4 CalculateLightContrib(vec3 vertexMaterialColour, vec3 outVertexNormal, vec3 vertexWorldPos, vec4 vertexSpecular);
	 
/*Shader features. Users are able to add or combine shader outputs by appending ShaderFeatures to a gameobject.*/	 
const int MAX_SHADER_FEATURES = 30;
struct FragmentShaderInfo_s
{
	int shaderFeatureArrayCount;					/*How many shaders are in use. Ex: 2 shaders (vertex+colorize)*/
	int shaderFeatureArray[MAX_SHADER_FEATURES];	/*And array of IDs. Each shader has an ID, which is a function in the uber shader.*/
};
uniform FragmentShaderInfo_s fragmentShaderInfo;
//01 - ShaderDiscardByHeight
//02 - ShaderTexturizeByHeight
//06 - Texturize
//07 - TexturizeSkybox
//08 - DoNotLight
//09 - Colorize
//10 - Imposter
//11 - ReflectRefract
//12 - TextureOffset

void ShaderTexturize(out vec4 result);
void ShaderSkybox(out vec4 result);
void ShaderReflectRefract(out vec4 result);
void ShaderColorize(out vec4 result);
void ShaderImposter(out vec4 result);
void ShaderInterpolationColorize(out vec4 result);

uniform float discardByHeightThreshold;
void ShaderDiscardByHeight(out vec4 result);

uniform float modelHeight;
uniform float textureRatioArray[20];
void ShaderTexturizeByHeight(out vec4 result);

//Pass functions
const int MAX_PASSES = 30;
struct PassProcessInfo
{
	int passArrayCount;
	int passArray[MAX_SHADER_FEATURES];
};
uniform PassProcessInfo passProcessInfo;

void Pass_FBOTextureEmplacement(out vec4 result);
void Pass_InClassBlur(out vec4 result);
void Pass_m3(out vec4 result);
void Pass_DepthTest(out vec4 result);
void Pass_Blur(out vec4 result);
void Pass_NightVision(out vec4 result);
void Pass_TVStatic(out vec4 result);
void Pass_DepthTest2(out vec4 result);
void Pass_SinWave_Static(out vec4 result);
void Pass_SinWave_Wobbly(out vec4 result);

void main()  
{
	/*Main pass*/
	if(passProcessInfo.passArrayCount == 0)
	{
		//vec4 resultBuffer = vec4(0.0, 0.0, 0.0, 0.0);
		vec4 result = vec4(0.0, 0.0, 0.0, 0.0);
		for(int i = 0; i < fragmentShaderInfo.shaderFeatureArrayCount; i++)
		{
			/*Apply shader to vertex*/
			switch(fragmentShaderInfo.shaderFeatureArray[i])
			{
				case 0: /*Reserved*/ break;
				case 1: ShaderDiscardByHeight(result); break;	
				case 2:	ShaderTexturizeByHeight(result); break;		
				case 6: ShaderTexturize(result); break;
				case 7: ShaderSkybox(result);	break;
				case 9: ShaderColorize(result); break;
				case 10: ShaderImposter(result); break;
				case 11: ShaderReflectRefract(result); break;
				case 15: ShaderInterpolationColorize(result); break;
				default: break;
			}

			//resultBuffer += result;
			//result = vec4(0.0, 0.0, 0.0, 0.0);
		}

		//resultBuffer = resultBuffer / fragmentShaderInfo.shaderFeatureArrayCount;
		pixelColour.rgb = result.rgb;
		pixelColour.a = result.a;
	}
	else
	{
		/*Processes all the fullscreen texture passes and effectively blends them*/
		/*Users can add many passes for the same texture to combine effects*/
		vec4 resultBuffer = vec4(0.0, 0.0, 0.0, 1.0);
		vec4 result = vec4(0.0, 0.0, 0.0, 1.0);
		for(int i = 0; i < passProcessInfo.passArrayCount; i++)
		{
			result = vec4(0.0, 0.0, 0.0, 1.0);
			switch(passProcessInfo.passArray[i])
			{
				case 0: /*Reserved*/ break;
				case 1: Pass_FBOTextureEmplacement(result); break;
				case 2: Pass_InClassBlur(result); break;
				case 3: Pass_Blur(result); break;
				case 4: Pass_NightVision(result); break;
				case 5: Pass_TVStatic(result); break;
				case 6: Pass_DepthTest(result); break;
				case 7: Pass_SinWave_Wobbly(result); break;
				case 8: Pass_SinWave_Static(result); break;
				default: break;
			}

			resultBuffer += result;
		}

		resultBuffer = resultBuffer / passProcessInfo.passArrayCount;
		pixelColour.rgb = resultBuffer.rgb;
		pixelColour.a = 1.0f;
	}
}


vec4 CalculateLightContrib(vec3 vertexMaterialColour, vec3 outVertexNormal, vec3 vertexWorldPos, vec4 vertexSpecular)
{
	vec3 norm = normalize(outVertexNormal);
	
	vec4 finalObjectColour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	for (int index = 0; index < NUMBEROFLIGHTS; index++)
	{	
		// ********************************************************
		// is light "on"
		if (lightArray[index].param2.x == 0.0f)
		{	// it's off
			continue;
		}
		
		// Cast to an int (note with c'tor)
		int intLightType = int(lightArray[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if (intLightType == DIRECTIONAL_LIGHT_TYPE)		// = 2
		{
			// This is supposed to simulate sunlight. 
			// SO: 
			// -- There's ONLY direction, no position
			// -- Almost always, there's only 1 of these in a scene
			// Cheapest light to calculate. 

			vec3 lightContrib = lightArray[index].diffuse.rgb;
			
			// Get the dot product of the light and normalize
			float dotProduct = dot(-lightArray[index].direction.xyz, normalize(norm.xyz));	// -1 to 1

			dotProduct = max(0.0f, dotProduct);		// 0 to 1
		
			lightContrib *= dotProduct;		
			
			finalObjectColour.rgb += (vertexMaterialColour.rgb * lightArray[index].diffuse.rgb * lightContrib); 
									 //+ (materialSpecular.rgb * lightSpecularContrib.rgb);
			// NOTE: There isn't any attenuation, like with sunlight.
			// (This is part of the reason directional lights are fast to calculate)

			return finalObjectColour;		
		}
		
		// Assume it's a point light 
		// intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = lightArray[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);	
		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, outVertexNormal.xyz);	 
		
		dotProduct = max( 0.0f, dotProduct );	
		
		vec3 lightDiffuseContrib = dotProduct * lightArray[index].diffuse.rgb;
			

		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
			
		vec3 reflectVector = reflect(-lightVector, normalize(norm.xyz));

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object’s.
		float objectSpecularPower = vertexSpecular.w; 
		
//		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
//			                   * vertexSpecular.rgb;	//* lightArray[lightIndex].Specular.rgb
		lightSpecularContrib = pow(max(0.0f, dot(eyeVector, reflectVector)), objectSpecularPower) * lightArray[index].specular.rgb;
							   
		// Attenuation
		float attenuation = 1.0f / 
				( lightArray[index].atten.x + 										
				  lightArray[index].atten.y * distanceToLight +						
				  lightArray[index].atten.z * distanceToLight*distanceToLight );  	
				  
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
				
		// But is it a spot light
		if ( intLightType == SPOT_LIGHT_TYPE )		// = 1
		{	
			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - lightArray[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
					= dot( vertexToLight.xyz, lightArray[index].direction.xyz );
					
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(lightArray[index].param1.z));
			float innerConeAngleCos = cos(radians(lightArray[index].param1.y));
							
			// Is it completely outside of the spot?
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
						
		}
		
		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb) + (vertexSpecular.rgb  * lightSpecularContrib.rgb );
	}
	
	finalObjectColour.a = 1.0f;
	
	return finalObjectColour;
}
//This shader interpolates between two diffuse colors over time.
void ShaderInterpolationColorize(out vec4 result)
{
    float gain = abs(sin(timeInfo.runTime));
	//Interpolates colors over time
    vec4 colorBlend = mix(interpolatinColorize.colorA, interpolatinColorize.colorB, gain);

	vec4 outColour = CalculateLightContrib(colorBlend.rgb, outVertexNormal.xyz, outVertexWorldLocation.xyz, specularColor);		
	result = outColour;
	result.a = colorBlend.a;
}
//This shader wraps an object with a cubemap.
void ShaderSkybox(out vec4 result)
{
	vec3 skyColour = texture(textureProcessInfo.skybox, outVertexNormal.xyz).rgb;
	result.rgb = skyColour.rgb;
	result.a = 1.0f;	
}
void ShaderReflectRefract(out vec4 result)
{
	vec3 texture_RGB = texture(textureProcessInfo.textureSample[textureIndex], outVertexTexture.st).rgb;						
	float texture_ratio = 1.0f;
	vec3 texRGB = (texture_ratio * texture_RGB);

	vec3 eyeVector = eyeLocation.xyz - outVertexWorldLocation.xyz;
	eyeVector = normalize(eyeVector);

	//vec4 outColour = CalculateLightContrib(texRGB.rgb, outVertexNormal.xyz, outVertexWorldLocation.xyz, specularColor);
	
	vec3 reflectVector = reflect(eyeVector, outVertexNormal.xyz);
	vec3 refractVector = refract(eyeVector, outVertexNormal.xyz, 1.0f);

	vec3 reflectColour = texture(textureProcessInfo.skybox, reflectVector.xyz).rgb;
	vec3 refractColour = texture(textureProcessInfo.skybox, refractVector.xyz).rgb;

	vec3 finalColour = 0.0f * reflectColour + 1.0f * refractColour;

	result.rgb = finalColour.rgb;
	result.a = diffuseColour.a;
}
//This shader covers an object with a single diffuse and specular. An option to ignore the light is provided.
void ShaderColorize(out vec4 result)
{
	vec4 outColour = vec4(0.0, 0.0, 0.0, 1.0);
	//Checks if the pixel is affected by light or not.
	if(colorizeInfo.isLit)
	{
		//Lits up the pixel
		outColour = CalculateLightContrib(diffuseColour.rgb, outVertexNormal.xyz, outVertexWorldLocation.xyz, specularColor);	
	}
	else
	{
		//No light, just returns the diffuse color
		outColour = diffuseColour;
	}
										
	result = outColour;
	result.a = diffuseColour.a;
}
//This shader is used for imposter objects.
void ShaderImposter(out vec4 result)
{
	//Blends all textures by default.
	vec3 texRGB = vec3(0.0, 0.0, 0.0);
	for(int i = 0; i < textureProcessInfo.textureCount; i++)
	{
		vec3 temptexRGB = texture(textureProcessInfo.textureSample[i], outVertexTexture.st * textureProcessInfo.tiling + textureProcessInfo.uvoffset).rgb;
		texRGB += temptexRGB;
	}
	texRGB = texRGB / textureProcessInfo.textureCount;

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
void ShaderDiscardByHeight(out vec4 result)
{
	if(outVertexWorldLocation.y <= discardByHeightThreshold)
	{
		discard;
	}
}
void ShaderTexturizeByHeight(out vec4 result)
{
	for(int i = textureProcessInfo.textureCount - 1; i > -1; i--)
	{
		//If the height is between a threshold (ex: 50% and 100% - where 70% would be valid), draw texture and return
		if(outVertexWorldLocation.y > (textureRatioArray[i-1]) * modelHeight && outVertexWorldLocation.y <= (textureRatioArray[i]) * modelHeight)
		{
			vec3 texture_RGB = texture(textureProcessInfo.textureSample[textureIndex + i], outVertexTexture.st).rgb;
			float texture_ratio = 1.0f;
			vec3 texRGB = (texture_ratio * texture_RGB);
			
			vec4 outColour = CalculateLightContrib(texRGB.rgb, outVertexNormal.xyz, outVertexWorldLocation.xyz, specularColor);
							
			result = outColour;
			result.a = diffuseColour.a;
			return;
		}
	}
}
//This shader covers an object with a blend of materials. This provides options to use transparencies, tiling and offsetting.
void ShaderTexturize(out vec4 result)
{	
	/*Blends all textures by default.*/
	vec3 texRGB = vec3(0.0, 0.0, 0.0);
	for(int i = 0; i < textureProcessInfo.textureCount; i++)
	{
		vec3 temptexRGB = texture(textureProcessInfo.textureSample[i], outVertexTexture.st * textureProcessInfo.tiling + textureProcessInfo.uvoffset).rgb;
		texRGB += temptexRGB;
	}
	texRGB = texRGB / textureProcessInfo.textureCount;

	switch(textureProcessInfo.transparency)
	{	
		/*Alpha transparency support*/
		default:
		case 0:
		{
			/*Calculates the light on top of the blend*/
			vec4 outColour = CalculateLightContrib(texRGB.rgb, outVertexNormal.xyz, outVertexWorldLocation.xyz, specularColor);

			result = outColour;
			/*If the diffuse color is lesser than 1.0f, then the result will also include transparency*/
			result.a = diffuseColour.a;
		}
		break;
		/*Discard transparency support*/
		case 1:
		{
			//Do not draw	
			//if (texRGB.r <= diffuseColour.a)  //'a' becomes the discard threshold
			//{
			//	discard;
			//}
			
			if(texRGB.rgb == vec3(0.0, 0.0, 0.0))
			{
				discard;
			}

			/*Calculates the light on top of the blend*/
			vec4 outColour = CalculateLightContrib(texRGB.rgb, outVertexNormal.xyz, outVertexWorldLocation.xyz, specularColor);
							
			result = outColour;
			result.a = 1.0f;
		}
		break;
	}
}

void Pass_FBOTextureEmplacement(out vec4 result)
{
	/*Just replace texture with whatever is in the FBO*/
	vec3 texRGB = texture(textureProcessInfo.fullscreenTexture, outVertexTexture.st).rgb;

	result = vec4(texRGB, 1.0);
}

void Pass_InClassBlur(out vec4 result)
{
	const float bo = 0.005f;		// For "blurr offset"
		
	// *  *  *
	// *  O  *
	// *  *  *
		
	// This is a Gaussian Kernel (Goo
	// * * * * *          *
	// * * * * *          *
	// * * O * *		**O**
	// * * * * *          *
	// * * * * *          *
	
	vec2 textCoords = outVertexTexture.st;
	//vec2 textCoords = gl_FragCoord.xy / screenResolution.xy;
	//vec3 texRGB = texture(textureProcessInfo.fullscreenTexture, textCoords).rgb;

	vec2 uvs = textCoords;			// Make a copy of the texture coords
		
//	uvs.s = outVertexWorldLocation.x / 50.0f;
//	uvs.t = outVertexWorldLocation.y / 50.0f;
		
	vec3 texRGB1 = texture( textureProcessInfo.fullscreenTexture, vec2(uvs.s + 0.0f, uvs.t + 0.0f) ).rgb;
		
	vec3 texRGB2 = texture( textureProcessInfo.fullscreenTexture, vec2(uvs.s - bo, uvs.t + 0.0f) ).rgb;
	vec3 texRGB3 = texture( textureProcessInfo.fullscreenTexture, vec2(uvs.s + bo, uvs.t + 0.0f) ).rgb;
	vec3 texRGB4 = texture( textureProcessInfo.fullscreenTexture, vec2(uvs.s + 0.0f, uvs.t - bo) ).rgb;
	vec3 texRGB5 = texture( textureProcessInfo.fullscreenTexture, vec2(uvs.s + 0.0f, uvs.t + bo) ).rgb;
		
	vec3 RGB = 0.5f * texRGB1 +
		        0.125f * texRGB2 +
				0.125f * texRGB3 +
				0.125f * texRGB4 +
				0.125f * texRGB5;

	result = vec4(RGB, 1.0f);
}

void Pass_m3(out vec4 result)
{
	// This will calculate the screen texture coordinates based 
	// on what's actually being rendered on the screen. 
	// So you just need to FILL the ENTIRE screen with something.
	vec2 textCoords = vec2( gl_FragCoord.x / screenResolution.x, gl_FragCoord.y / screenResolution.y );
	vec3 texRGB = texture( textureProcessInfo.fullscreenTexture, textCoords.st ).rgb;

	result = vec4(texRGB, 1.0);
}

void Pass_DepthTest(out vec4 result)
{
	vec2 textCoords = outVertexTexture.st;
	//vec2 textCoords = gl_FragCoord.xy / screenResolution.xy;
	vec3 texRGB = texture(textureProcessInfo.fullscreenTexture, textCoords).rgb;

	float depthValue = texRGB.r;
//	depthValue /= 10.0f;

	result = vec4(depthValue, depthValue, depthValue, 1.0);
}

void Pass_DepthTest2(out vec4 result)
{
	//Near clipping plane = clippingPlaneInfo.x
	//Far clipping plane = clippingPlaneInfo.y

//	float z_ndc = gl_FragCoord.z * 2.0 - 1.0;
//	float z_clip = z_ndc / gl_FragCoord.w;
//	float depthVal = (z_clip + clippingPlaneInfo.y) / (clippingPlaneInfo.y + clippingPlaneInfo.x);

	float z_ndc = gl_FragCoord.z * 2.0 - 1.0;
	float depthVal = (2.0 * clippingPlaneInfo.x * clippingPlaneInfo.y) / (clippingPlaneInfo.y * clippingPlaneInfo.x - z_ndc * (clippingPlaneInfo.y - clippingPlaneInfo.x));
	depthVal /= clippingPlaneInfo.y;

	result = vec4(depthVal, depthVal, depthVal, 1.0);
}

float normpdf(in float x, in float sigma)
{
	return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

/*Creates a single pass blur effect using the gaussian model*/
/*Similar to the code provided in class, but this generates the kernels instead of having them pre-made.*/
void Pass_Blur(out vec4 result)
{
	//Adapted from: https://www.shadertoy.com/view/XdfGDH

	vec2 textCoords = outVertexTexture.st;
	//vec2 textCoords = gl_FragCoord.xy / screenResolution.xy;
	vec3 texRGB = texture(textureProcessInfo.fullscreenTexture, textCoords).rgb;

	//declare stuff
	const int mSize = 11;
	const int kSize = (mSize-1)/2;
	float kernel[mSize];
	vec3 final_colour = vec3(0.0);
		
	/*Generates a one dimensional kernel array.*/
	float sigma = 7.0;
	float Z = 0.0;
	for (int j = 0; j <= kSize; ++j)
	{
		kernel[kSize+j] = kernel[kSize-j] = normpdf(float(j), sigma);
	}
		
	//get the normalization factor (as the gaussian has been clamped)
	for (int j = 0; j < mSize; ++j)
	{
		Z += kernel[j];
	}
		
	/*Offsets the pixels for the blur effect*/
	for (int i=-kSize; i <= kSize; ++i)
	{
		for (int j=-kSize; j <= kSize; ++j)
		{
			final_colour += kernel[kSize+j]*kernel[kSize+i] * texture(textureProcessInfo.fullscreenTexture, (gl_FragCoord.xy + vec2(float(i),float(j))) / screenResolution.xy).rgb;
			//*texture(textureProcessInfo.fullscreenTexture, (gl_FragCoord.xy + vec2(float(i),float(j))) / screenResolution.xy).rgb;
		}
	}
				
	result = vec4(final_colour/(Z*Z), 1.0);
}

/*This draws a night vision effect, where the outer area is darkened to mimic a scope.*/
void Pass_NightVision(out vec4 result)
{   
	/*Globals*/
	vec3 luminance_tone = vec3(0.20, 0.75, 0.07);
	float fade = 0.05;
	float lineWidth = 0.0008;
	float lineSpeed = 0.001;

	vec2 textCoords = outVertexTexture.st;
	//vec2 textCoords = gl_FragCoord.xy / screenResolution.xy;
	vec3 texRGB = texture(textureProcessInfo.fullscreenTexture, textCoords).rgb;
    float screenRatio = screenResolution.y / screenResolution.x;
    
	/*Find the a gray color based on the texture and a fixed luminance value*/
    float gray = pow(dot(texRGB.rgb, luminance_tone), 0.3);
	/*The actual color of the screeen will be a tone of say green multiplied by the grayscale*/
    vec3 color = vec3(0.3, 1.0, 0.3) * gray;
    /*This creates the effect of a lens, in which the lines in the glass appear to be moving.*/
	/*'Interpolates' on the Y axis so we get the jagged lines.*/
    if(mod(textCoords.y + timeInfo.deltaTime * lineSpeed, lineWidth) < lineWidth * 0.5)
	{
		/*Increment the color so we can tell one line from another*/
        color *= 0.8;
	}
   
	result = vec4(color, 1.0);
}

void Pass_TVStatic(out vec4 result)
{
	/*The 'granularity' of the tv static.*/
	const float staticValue = 2.0; 

	/*The gain is an interpolated value that is used to generate the static pixel.*/
	float gain = staticValue + (timeInfo.runTime * 0.1);
    vec2 r = (gain * sin(gain * gl_FragCoord.xy));
   
	result = vec4(fract(r.x * r.y * (1.0 + gl_FragCoord.x)));
}

void Pass_SinWave_Wobbly(out vec4 result)
{
	vec2 textCoords = outVertexTexture.st;
	//vec2 textCoords = gl_FragCoord.xy / screenResolution.xy;
	vec3 texRGB = texture(textureProcessInfo.fullscreenTexture, textCoords).rgb;

	/*Change X by a frequency of 10f, while using the sin of Y to oscillate.*/
	/*RunTime will produce a 'drunk' effect*/
	textCoords.x = textCoords.x + (sin(textCoords.y * 10.0 + timeInfo.runTime) / 10.0);		
	/*DeltaTime will produce a 'carnival-mirror' effect*/
	//textCoords.x = textCoords.x + (sin(textCoords.y * 10.0 + timeInfo.runTime) / 10.0);		
	vec4 color = texture(textureProcessInfo.fullscreenTexture, textCoords);	

	result = color;
}

void Pass_SinWave_Static(out vec4 result)
{
	vec2 textCoords = outVertexTexture.st;
	//vec2 textCoords = gl_FragCoord.xy / screenResolution.xy;
	vec3 texRGB = texture(textureProcessInfo.fullscreenTexture, textCoords).rgb;

	/*Change X by a frequency of 10f, while using the sin of Y to oscillate.*/
	/*RunTime will produce a 'drunk' effect*/
	//textCoords.x = textCoords.x + (sin(textCoords.y * 10.0 + timeInfo.runTime) / 10.0);		
	/*DeltaTime will produce a 'carnival-mirror' effect*/
	textCoords.x = textCoords.x + (sin(textCoords.y * 10.0 + timeInfo.deltaTime) / 10.0);		
	vec4 color = texture(textureProcessInfo.fullscreenTexture, textCoords);	

	result = color;
}