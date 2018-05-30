#include "Shader.h"

#include <GLAD/glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
//#include <fstream>
//#include <sstream>
#include <iostream>
#include <string>

#pragma once

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};
class Mesh
{
public:
	Mesh(std::vector<Vertex> iVertices, std::vector<unsigned int> iIndices, std::vector<Texture> iTextures);

	void Draw(Shader shader);

	const std::vector<Vertex>& GetVertices() const { return mVertices; };
	const std::vector<unsigned int>& GetIndices() const { return mIndices; };

private:
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<Texture> mTextures;
	unsigned int VAO, VBO, EBO;

	void Setup();
};