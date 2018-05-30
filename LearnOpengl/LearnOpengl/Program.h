#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "SceneManager.h"
#include "Utility.h"

#include <GLAD/glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#pragma once



class Program
{
public:
	Program() {};
	~Program() {};
	
	/*Initialize*/
	bool Init();
	/*Program loop*/
	void Run();
	/*Destroy window and program*/ 
	void Destroy();
	
	/*Process Input*/
	void ProcessInput(GLFWwindow* window);

	/*Bind buffers*/
	void BindBuffer();

	/*Get window*/
	GLFWwindow* GetWindow() const { return mpWindow; } ;

	/*Callback when window size changes*/
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void MouseCallBack(GLFWwindow* window, double xpos, double ypos);
	static void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset);
private:
	static Camera* mpCamera;

	GLFWwindow* mpWindow;
	std::vector<Shader> mShaderList;
	int cntShaderIndex;
	static float width, height, farDistance, nearDistance;
	unsigned int VBO, VAO, EBO;
	float mDeltaTime = 0.0f;
	float mLastFrame = 0.0f;
};
