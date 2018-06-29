#include "Mesh.h"
#include "Program.h"

Mesh::Mesh(std::vector<Vertex> iVertices, std::vector<unsigned int> iIndices, std::vector<Texture> iTextures):
	mVertices(iVertices), mIndices(iIndices),mTextures(iTextures)
{
	Setup();
}

void Mesh::Draw(Shader shader)
{
	unsigned int numDiffuse = 1;
	unsigned int numSpecular = 1;


	//draw
	glBindVertexArray(VAO);
	for (int i = 0; i < mTextures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = mTextures[i].type;
		if (name == "textureDiffuse")
			number = std::to_string(numSpecular++);
		else if (name == "textureSpecular")
			number = std::to_string(numSpecular++);

		shader.Set(("material." + name + number).c_str(), (float)i);

		glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	glActiveTexture(GL_TEXTURE2);
	shader.Set("shadowMap", 2);
	glBindTexture(GL_TEXTURE_2D, Program::Instance()->mDepthMap);
	glActiveTexture(GL_TEXTURE0);

	// normal
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);

	//instance
	//glDrawElementsInstanced(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0, 100);
	glBindVertexArray(0);
}

void Mesh::Setup()
{
	//generate 
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	static bool firstEnter = true;
	static unsigned int instanceVBO;
	static float translations[100][3];

	if (firstEnter)
	{
		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 10; ++j)
			{
				translations[i * 10 + j][0] = i * 10;
				translations[i * 10 + j][1] = j * 10;
				translations[i * 10 + j][2] = 0;

			}
		}
		firstEnter = false;
	}


	//bind
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

	// instance
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 100, &translations[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	//vertex normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	//vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// instance
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(3, 1);

	//unbind
	glBindVertexArray(0);

}
