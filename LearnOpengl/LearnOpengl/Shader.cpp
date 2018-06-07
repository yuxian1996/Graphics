#include "Shader.h"

bool Shader::Create(const GLchar* vertexShaderPath, const GLchar* fragShaderPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vertexShaderFile;
	std::ifstream fragShaderFile;

	//ensure ifstream can throw exceptions for debug
	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		//open files
		vertexShaderFile.open(vertexShaderPath);
		fragShaderFile.open(fragShaderPath);
		std::stringstream vertexShaderStream, fragShaderStream;

		//read buffer to streams
		vertexShaderStream << vertexShaderFile.rdbuf();
		fragShaderStream << fragShaderFile.rdbuf();

		//close file handlers
		vertexShaderFile.close();
		fragShaderFile.clear();

		//convert stream to string
		vertexCode = vertexShaderStream.str();
		fragmentCode = fragShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vertexShaderCode = vertexCode.c_str();
	const char* fragShaderCode = fragmentCode.c_str();

	//create vertex shader
	mVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(mVertexShader, 1, &vertexShaderCode, NULL);

	//create fragment shader
	mFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(mFragShader, 1, &fragShaderCode, NULL);

	if (!Compile() || !Link())
		return false;

	glDeleteShader(mVertexShader);
	glDeleteShader(mFragShader);

	return true;
}

void Shader::Use()
{
	glUseProgram(ID);
}

void Shader::Set(const std::string & name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

void Shader::Set(const std::string & name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::Set(const std::string & name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::Set(const std::string & name, const glm::mat4 & mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::Set(const std::string & name, const glm::vec3 & vector) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &vector[0]);
}

void Shader::Set(const std::string& name, Light* light) const
{
	light->SetToShader(name, *this);

	if (dynamic_cast<DirectionalLight*>(light) != NULL)
		Set(name + ".type", LightType::DIRECTIONALLIGHT);
	else if(dynamic_cast<PointLight*>(light) != NULL)
		Set(name + ".type", LightType::POINTLIGHT);
	else if(dynamic_cast<SpotLight*>(light) !=NULL)
		Set(name + ".type", LightType::SPOTLIGHT);

}

bool Shader::Compile()
{
	//compile vertex shader
	glCompileShader(mVertexShader);

	int success;
	char infoLog[512];
	//check if success
	glGetShaderiv(mVertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		//log information
		glGetShaderInfoLog(mVertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}

	//compile fragment shader
	glCompileShader(mFragShader);
	//check if success
	glGetShaderiv(mFragShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		//log information
		glGetShaderInfoLog(mFragShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

bool Shader::Link()
{
	//link shader program
	ID = glCreateProgram();
	glAttachShader(ID, mVertexShader);
	glAttachShader(ID, mFragShader);
	glLinkProgram(ID);

	int success;
	char infoLog[512];
	//check if success and log information
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

