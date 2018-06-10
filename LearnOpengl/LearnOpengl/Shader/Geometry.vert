#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT
{
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
} vs_out;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	//TexCoords = texCoords;
	vs_out.texCoords = texCoords;
	vs_out.fragPos = vec3(transform * vec4(pos, 1.0));
	vs_out.normal = mat3(transpose(inverse(transform))) * normal;

	gl_Position = projection * view * vec4(vs_out.fragPos, 1.0);
}