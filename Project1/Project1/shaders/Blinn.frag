#version 330 core

in vec2 uv;
in vec3 pos_ws;
in vec3 normal_cs;
in vec3 eye_cs;



uniform mat4 V;
uniform int lightCount;
uniform sampler2D diffuseTexture;


out vec3 color;

void main(){

	color = vec3(0);
}



