//#include <glm/glm/gtc/matrix_transform.hpp>
//#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <string>
#pragma once

enum LightType { DIRECTIONALLIGHT, POINTLIGHT, SPOTLIGHT };
//struct Light
//{
//	LightType type;
//	glm::vec3 position;
//	glm::vec3 direction;
//	glm::vec3 ambient;
//	glm::vec3 diffuse;
//	glm::vec3 specular;
//
//	float constant;
//	float linear;
//	float quadratic;
//
//	float cutOff;
//	float outerCutOff;
//
//};

class Shader;
class Light
{
public:
	Light() {};
	Light(const glm::vec3& iAmbient, const glm::vec3& iDiffuse, const glm::vec3& iSpecular):
		mAmbient(iAmbient), mDiffuse(iDiffuse), mSpecular(iSpecular)
	{};
	virtual ~Light() {};

	/* Get members*/
	glm::vec3 GetAmbient() { return mAmbient; };
	glm::vec3 GetDiffuse() { return mDiffuse; };
	glm::vec3 GetSpecular() { return mSpecular; };

	virtual void SetToShader(const std::string& name, const Shader& shader) const;

protected:
	glm::vec3 mAmbient;
	glm::vec3 mDiffuse;
	glm::vec3 mSpecular;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight() {};
	DirectionalLight(const glm::vec3& iAmbient, const glm::vec3& iDiffuse, const glm::vec3& iSpecular, const glm::vec3& iDirection) :
		Light(iAmbient, iDiffuse, iSpecular), mDirection(iDirection)
	{};
	~DirectionalLight() {};

	void SetToShader(const std::string& name, const Shader& shader) const override;
private:
	glm::vec3 mDirection;
};

class PointLight : public Light
{
public:
	PointLight() {};
	PointLight(const glm::vec3& iAmbient,const glm::vec3& iDiffuse,const glm::vec3& iSpecular, const glm::vec3& iPosition, float iConstant, float iLinear, float iQuadratic) :
		Light(iAmbient, iDiffuse, iSpecular), mPosition(iPosition), mConstant(iConstant), mLinear(iLinear), mQuadratic(iQuadratic)
	{};
	~PointLight() {};

	void SetToShader(const std::string& name, const Shader& shader) const override;

private:
	glm::vec3 mPosition;
	float mConstant;
	float mLinear;
	float mQuadratic;

};

class SpotLight : public Light
{
public:
	SpotLight() {};
	SpotLight(const glm::vec3& iAmbient, const glm::vec3& iDiffuse, const glm::vec3& iSpecular, const glm::vec3& iDirection, const glm::vec3& iPosition,
		float iConstant, float iLinear, float iQuadratic, float iCutOff, float iOuterCutOff) :
		Light(iAmbient, iDiffuse, iSpecular), mDirection(iDirection), mPosition(iPosition), mConstant(iConstant),
		mLinear(iLinear), mQuadratic(iQuadratic), mCutOff(iCutOff), mOuterCutOff(iOuterCutOff)
	{};

	~SpotLight() {};

	void SetToShader(const std::string& name, const Shader& shader)const override;

private:
	glm::vec3 mDirection;
	glm::vec3 mPosition;
	float mConstant;
	float mLinear;
	float mQuadratic;
	float mCutOff;
	float mOuterCutOff;
};