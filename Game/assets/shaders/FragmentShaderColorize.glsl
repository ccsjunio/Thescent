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

void ShaderColorize(out vec4 result);

void main()  
{
	vec4 result = vec4(0.0, 0.0, 0.0, 0.0);
	ShaderColorize(result);		
	pixelColour.rgb = result.rgb;
	pixelColour.a = result.a;
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