#pragma once
#include "Shader.h"

#include <vector>
#include <string>

class Cubemap
{
public:
	Cubemap() = delete;
	~Cubemap();
	Cubemap(const std::vector<std::string>& iFaces);

	/* Set shader*/
	void SetShader(const Shader& shader) { mShader = shader; }
	/* GetShader*/
	Shader GetShader() { return mShader; }

	/* Draw cubemap*/
	void Draw();
	
private:
	static float cubmapVertices[];
	unsigned int mTexture, VAO, VBO;
	Shader mShader;

	void LoadCubemap(const std::vector<std::string>& iFaces);
	void BindBuffer();
};