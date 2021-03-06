#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "SceneManager.h"
#include "Utility.h"
#include "Cubemap.h"

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
	enum ShaderType
	{
		LIGHT,
		DEPTH,
		OUTLINE,
		POST_PROCESS,
		GEOMETRY,
		INSTANC,
		SHADOW_MAP
	};

	Program() { sInstance = this; };
	~Program() {};
	
	/*Initialize*/
	bool Init();
	/*Program loop*/
	void Run();
	/*Destroy window and program*/ 
	void Destroy();
	
	/*Process Input*/
	void ProcessInput(GLFWwindow* window);

	/*Get window*/
	GLFWwindow* GetWindow() const { return mpWindow; } ;

	/*Callback when window size changes*/
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void MouseCallBack(GLFWwindow* window, double xpos, double ypos);
	static void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset);
	static Camera* GetCamera() { return mpCamera; }
	static glm::mat4 GetProjectionMatrix() {
		return glm::perspective(glm::radians(mpCamera->GetZoom()), (float)width / (float)height, 0.1f, 100.0f);}

	static Program* Instance() { return sInstance; };

	unsigned int mDepthMapFBO, mDepthMap;

private:
	static Camera* mpCamera;
	static Program* sInstance;

	GLFWwindow* mpWindow;
	std::vector<Shader> mShaderList;
	int cntModeIndex;
	static float width, height, farDistance, nearDistance;
	unsigned int mEffectType = 0;
	float mDeltaTime = 0.0f;
	float mLastFrame = 0.0f;
	Cubemap* mSkybox;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	void LightMode(Light* light);
	void DepthTest();
	void OutlineMode(Light* light);
	void FaceCullingMode(Light* light);
	void PostProcessMode(Light* light);
	void GeometryUsedMode(Light* light);
	void InstancingMode(Light* light);
};
