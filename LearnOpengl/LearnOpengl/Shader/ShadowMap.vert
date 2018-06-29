#version 330 core
layout (location = 0) in vec3 pos;

uniform mat4 transform;
uniform mat4 lightSpaceMatrix;

void main()
{
	gl_Position = lightSpaceMatrix * transform * vec4(pos, 1.0);
}