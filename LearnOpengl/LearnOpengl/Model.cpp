#include "Model.h"


void Model::Draw()
{
	for (int i = 0; i < mMeshes.size(); i++)
		mMeshes[i].Draw(mShader);
}

void Model::Load(std::string iPath)
{
	//import and read file
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(iPath, aiProcess_Triangulate | aiProcess_FlipUVs);

	//check if succeed
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	//set directory
	mDirectory = iPath.substr(0, iPath.find_last_of('/'));

	//process root node
	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode * ipNode, const aiScene * ipScene)
{
	//process all the meshes of this node
	for (int i = 0; i < ipNode->mNumMeshes; i++)
	{
		aiMesh *mesh = ipScene->mMeshes[ipNode->mMeshes[i]];
		mMeshes.push_back(processMesh(mesh, ipScene));
	}

	//process all the child node
	for (int i = 0; i < ipNode->mNumChildren; i++)
		ProcessNode(ipNode->mChildren[i], ipScene);
}

Mesh Model::processMesh(aiMesh * ipMesh, const aiScene * ipScene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//set each vertex
	for (int i = 0; i < ipMesh->mNumVertices; i++)
	{
		Vertex vertex;
		//position
		vertex.Position.x = ipMesh->mVertices[i].x;
		vertex.Position.y = ipMesh->mVertices[i].y;
		vertex.Position.z = ipMesh->mVertices[i].z;
		//normal
		vertex.Normal.x = ipMesh->mNormals[i].x;
		vertex.Normal.y = ipMesh->mNormals[i].y;
		vertex.Normal.z = ipMesh->mNormals[i].z;

		//only check the first texture, can be up to 8
		if (ipMesh->mTextureCoords[0])
		{
			vertex.TexCoords.x = ipMesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = ipMesh->mTextureCoords[0][i].y;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		
		vertices.push_back(vertex);
	}

	//set each index
	for (int i = 0; i < ipMesh->mNumFaces; i++)
	{
		aiFace face = ipMesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//set material
	if (ipMesh->mMaterialIndex >= 0)
	{
		//load material
		aiMaterial* material = ipScene->mMaterials[ipMesh->mMaterialIndex];
		//load and add diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "textureDiffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		//load and add specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "textureSpecular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial * ipMaterial, aiTextureType iType, std::string iTypeName)
{
	std::vector<Texture> textures;
	//load textures
	for (int i = 0; i < ipMaterial->GetTextureCount(iType); i++)
	{
		//get texture path
		aiString string;
		ipMaterial->GetTexture(iType, i, &string);
		//get texture
		Texture texture;
		texture.type = iTypeName;
		texture.id = TextureFromFile(string.C_Str(), mDirectory);
		texture.path = string.C_Str();
		textures.push_back(texture);
	}
	return textures;
}

unsigned int Model::TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}