#include "Light.h"
#include "Shader.h"

void Light::SetToShader(const std::string& name, const Shader& shader) const
{
	shader.Set(name + ".ambient", mAmbient);
	shader.Set(name + ".diffuse", mDiffuse);
	shader.Set(name + ".specular", mSpecular);
}

void DirectionalLight::SetToShader(const std::string& name, const Shader& shader) const
{
	Light::SetToShader(name, shader);
	shader.Set(name + ".direction", mDirection);
}

void PointLight::SetToShader(const std::string& name, const Shader& shader)const
{
	Light::SetToShader(name, shader);

	shader.Set(name + ".position", mPosition);

	shader.Set(name + ".constant", mConstant);
	shader.Set(name + ".linear", mLinear);
	shader.Set(name + ".quadratic", mQuadratic);
}

void SpotLight::SetToShader(const std::string& name, const Shader& shader) const
{
	Light::SetToShader(name, shader);

	shader.Set(name + ".direction", mDirection);
	shader.Set(name + ".position", mPosition);

	shader.Set(name + ".constant", mConstant);
	shader.Set(name + ".linear", mLinear);
	shader.Set(name + ".quadratic", mQuadratic);

	shader.Set(name + ".cutOff", mCutOff);
	shader.Set(name + ".outerCutOff", mOuterCutOff);

}

