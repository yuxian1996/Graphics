#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 offset;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoords = texCoords;
	FragPos = vec3(transform * vec4(pos + offset, 1.0));

	Normal = mat3(transpose(inverse(transform))) * normal;
	gl_Position = projection * view * vec4(FragPos, 1.0);
}