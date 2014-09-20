#version 330 core

layout(location = 0) vec3 vertPos_ms;
layout(location = 1) vec2 uv_ms;
layout(location = 2) vec3 normal_ms;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;



void main()
{
	gl_Position = MVP *  vec4(vertPos_ms,1);
	

}
