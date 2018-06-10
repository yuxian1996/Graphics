#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
} gs_in[];

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform float time;

vec4 Explode(vec4 position, vec3 normal)
{
	float magnitude = 1;
	vec3 direction = normal*((sin(time) + 1.0) / 2.0) * magnitude;
	return position + vec4(direction, 0.0);
}

void main()
{
	gl_Position = Explode(gl_in[0].gl_Position, -gs_in[0].normal);
	TexCoords = gs_in[0].texCoords;
	Normal = gs_in[0].normal;
	//FragPos = gs_in[0].fragPos;
	FragPos = gl_Position.xyz;
	EmitVertex();

	gl_Position = Explode(gl_in[1].gl_Position, -gs_in[1].normal);
	TexCoords = gs_in[1].texCoords;
	Normal = gs_in[1].normal;
	//FragPos = gs_in[1].fragPos;
	FragPos = gl_Position.xyz;
	EmitVertex();

	gl_Position = Explode(gl_in[2].gl_Position, -gs_in[2].normal);
	TexCoords = gs_in[2].texCoords;
	Normal = gs_in[2].normal;
	//FragPos = gs_in[2].fragPos;
	FragPos = gl_Position.xyz;
	EmitVertex();

	EndPrimitive();
}
