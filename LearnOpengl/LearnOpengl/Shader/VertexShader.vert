#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec4 FragPosLightSpace;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
	TexCoords = texCoords;
	FragPos = vec3(transform * vec4(pos, 1.0));

	Normal = mat3(transpose(inverse(transform))) * normal;
	gl_Position = projection * view * vec4(FragPos, 1.0);
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos , 1.0);
}