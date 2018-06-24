#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
uniform samplerCube skybox;

void main()
{
	FragColor = texture(skybox, TexCoords);
		float gamma = 2.2;
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));

}