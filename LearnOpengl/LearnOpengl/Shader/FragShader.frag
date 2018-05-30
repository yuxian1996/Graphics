#version 330 core
out vec4 FragColor;

struct Material
{
	sampler2D  textureDiffuse1;
	sampler2D  textureSpecular1;
};

struct Light
{
	int type;      //0:directional, 1: point, 2: sopt
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	//point light
	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main()
{
	//FragColor = vec4(Normal, 1.0);
	vec3 lightDir;
	float attenuation = 1.0;
	if(light.type == 0 || light.type == 1)
	{
		if(light.type == 0)
			lightDir = normalize(-light.direction);
		else if(light.type == 1)
		{
			float distance = length(light.position - FragPos);
			attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
			lightDir = normalize(light.position - FragPos);
		}

		vec3 ambient = light.ambient * texture(material.textureDiffuse1, TexCoords).rgb;
		//diffuse
		vec3 norm = normalize(Normal);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light.diffuse * diff * texture(material.textureDiffuse1, TexCoords).rgb;
		//specular
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = light.specular * spec * texture(material.textureSpecular1, TexCoords).rgb;	

		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;

		vec3 result = ambient + diffuse + specular;

		FragColor = vec4(result, 1.0); 

	}
	else if(light.type == 2)
	{
		lightDir = normalize(light.position - FragPos);
		float distance = length(light.position - FragPos);
		attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

		float theta = dot(lightDir, normalize(-light.direction));
		float epsilon = light.cutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

		if(theta > light.outerCutOff)
		{
		vec3 ambient = light.ambient * texture(material.textureDiffuse1, TexCoords).rgb;
		//diffuse
		vec3 norm = normalize(Normal);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light.diffuse * diff * texture(material.textureDiffuse1, TexCoords).rgb;
		//specular
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = light.specular * spec * texture(material.textureSpecular1, TexCoords).rgb;	

		diffuse *= attenuation * intensity;
		specular *= attenuation * intensity;

		vec3 result = ambient + diffuse + specular;

		FragColor = vec4(result, 1.0); 

		}
		else
		{
			FragColor = vec4(light.ambient * texture(material.textureDiffuse1, TexCoords).rgb, 1.0);
		}


	}

}