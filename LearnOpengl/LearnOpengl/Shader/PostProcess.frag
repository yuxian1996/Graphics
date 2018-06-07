#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int type;

void main()
{
	vec3 color = texture(screenTexture, TexCoords).rgb;

	switch(type)
	{
		case 1:
		// inverse color
		FragColor = vec4(1.0 - color, 1.0);
		break;
		default:
		// no effect
		FragColor = vec4(color, 1.0);
		break;
	}
}