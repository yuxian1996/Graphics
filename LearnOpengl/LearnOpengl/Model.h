#include "Shader.h"
#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

#pragma once


class Model
{
public :
	Model(char *ipPath, std::string iName):mName(iName) { Load(ipPath);};
	void SetShader(const Shader& iShader) { mShader = iShader; };
	void Draw();

	const std::vector<Mesh>& GetAllMeshes() { return mMeshes; };

	const std::string GetName() const { return mName; };

private:
	std::vector<Mesh> mMeshes;
	std::string mDirectory;
	std::string mName;
	Shader mShader;

	void Load(std::string iPath);
	void ProcessNode(aiNode *ipNode, const aiScene *ipScene);
	Mesh processMesh(aiMesh *ipMesh, const aiScene *ipScene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *ipMaterial, aiTextureType iType, std::string iTypeName);
	unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

};