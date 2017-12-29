//#include <glm/glm/gtc/matrix_transform.hpp>
//#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/glm.hpp>
#pragma once

enum LightType { DIRECTIONALLIGHT, POINTLIGHT, SPOTLIGHT };

struct Light
{
	LightType type;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;

};