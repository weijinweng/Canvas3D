#version 330 core

layout(location = 0) out vec3 fragementDepth;

void main()
{
	fragementDepth = vec3(gl_FragCoord.z);
}