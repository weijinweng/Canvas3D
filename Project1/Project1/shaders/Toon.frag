#version 330 core
 
out vec4 fragmentColor;
 
struct Light
{
    vec4    position;
    vec4    ambient;
    vec4    diffuse;
    vec4    specular;
    float   constant_attenuation;
    float   linear_attenuation;
    float   quadratic_attenuation;
    vec3    spot_direction;
    float   spot_cutoff;
    float   spot_exponent;
	int type;
};
 
layout(shared) uniform Lights
{
    Light light[10];
} lights;
 
struct Material
{
    vec4    diffuse;
} material;
 
uniform int num_lights;

uniform sampler2D myTextureSampler;
 
uniform sampler2DShadow shadowTextures;

uniform mat4 DepthBiasMVP;

smooth in vec3 wPosition;
smooth in vec3 wNormal;
smooth in vec2 uv;
smooth in vec3 mPosition;

float directionLight(int lightID)
{
	vec3 lightDir = lights.light[lightID].position.xyz;
	float intensity = dot(lightDir, wNormal);
	
	return intensity;
}

float calcIntensity(int lightID)
{
	switch (lights.light[lightID].type)
	{
		case 0:
			return 0.0f;
		case 2: 
			return directionLight(lightID);
	}
	return 0.0f;
}


void main(void)
{
	material.diffuse = vec4(texture(myTextureSampler, uv).rgb,1);
	float multiplier = 1.0f;
	float intensity = 0.0;
	for(int i = 0; i < num_lights; i++)
	{
		intensity += calcIntensity(i);
	}
	
	if (intensity > 0.95)
		multiplier = 1.0f;
	else if (intensity > 0.5)
		multiplier = 0.7f;
	else if (intensity > 0.25)
		multiplier = 0.3f;
	else
		multiplier = 0.1f;
	fragmentColor = multiplier * material.diffuse;
}






















