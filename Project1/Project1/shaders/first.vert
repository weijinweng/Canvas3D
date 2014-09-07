#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout (location = 1) in vec2 uvC;
layout (location = 2) in vec3 normal;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;

out vec2 uv;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

void main(){
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	Position_worldspace = (M * vec4(vertexPosition_modelspace, 1)).xyz;
	
	vec3 vertexPosition_cameraspace = (V * M * vec4(vertexPosition_modelspace, 1)).xyz;
	
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace, 1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
	
	Normal_cameraspace = ( V*M * vec4(normal, 0)).xyz;		

	uv = uvC;
}