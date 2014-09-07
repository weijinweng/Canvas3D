# version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 uvC;
layout(location = 2) in vec3 normal;

out vec2 uv;



void main()
{
	gl_Position = vec4(vertexPosition_modelspace,1);
	uv = uvC;
}