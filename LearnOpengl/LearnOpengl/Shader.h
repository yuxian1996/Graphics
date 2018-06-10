#include "Light.h"

#include <GLAD/glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#pragma once

class Shader
{
public:
	Shader() {};
	~Shader() {};
	bool Create(const GLchar* vertexShader, const GLchar* fragShader, const GLchar* GeometryShader = nullptr);

	/*Use shader*/
	void Use();
	/*Get shader program id*/
	const unsigned int GetID() const { return ID; };

	/*Set uniform*/
	void Set(const std::string &name, bool value) const;
	void Set(const std::string &name, int value) const;
	void Set(const std::string &name, float value) const;
	void Set(const std::string &name, const glm::mat4  &mat) const;
	void Set(const std::string &name, const glm::vec3  &vector) const;
	void Set(const std::string& name, Light* light) const;

		
private:
	/*Compile shaders*/
	bool Compile();
	/*Link shaders*/
	bool Link();

	unsigned int mVertexShader;
	unsigned int mFragShader;
	int mGeometryShader = -1;
	unsigned int ID;

};
