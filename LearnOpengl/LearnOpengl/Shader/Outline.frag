#version 330 core
out vec4 FragColor;

void main()
{
	FragColor = vec4(0.04, 0.28, 0.26, 1.0);
		float gamma = 2.2;
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));

}