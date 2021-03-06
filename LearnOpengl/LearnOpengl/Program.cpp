#include "Program.h"

Camera* Program::mpCamera = NULL;
float Program::width = 800, Program::height = 600, Program::farDistance = 100.0f, Program::nearDistance = 0.1f;;
Program* Program::sInstance = nullptr;

//Callback
void Program::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Program::MouseCallBack(GLFWwindow * window, double xpos, double ypos)
{
	static float lastX = width / 2.0f;
	static float lastY = height / 2.0f;
	static bool firstMouse = false;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		//check raycast
		HitInfo info;
		//Utility::Raycast(glm::vec3(0.2f, 2, 5), glm::vec3(0,0,-1), 1000, info);

		float upAngle = (height / 2.0f - ypos) / (height / 2.0f) * 22.5f;
		float rightAngle = (width / 2.0f - xpos) / (width / 2.0f) * (22.5f * width / height);
		glm::vec3 direction = mpCamera->GetDirection();
		glm::mat4 rotation;
		rotation = glm::rotate(rotation, glm::radians(upAngle), glm::vec3(1, 0, 0));
		rotation = glm::rotate(rotation, glm::radians(rightAngle), glm::vec3(0, 1, 0));
		direction = rotation * glm::vec4(direction, 1.0f);
		//std::cout << direction.x << ", " << direction.y << ", " << direction.z << ", " << std::endl;

		SceneManager::Instance()->Raycast(mpCamera->GetPosition(), direction, 1000, info);

		if (info.hitObject != nullptr)
			std::cout << info.hitPosition.x << ", " << info.hitPosition.y << ", " << info.hitPosition.z << std::endl;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (firstMouse)
		{
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;

			lastX = xpos;
			lastY = ypos;

			mpCamera->Rotate(xoffset, yoffset);
		}
		else
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = true;
		}

	}
	else
		firstMouse = false;

}

void Program::ScrollCallBack(GLFWwindow * window, double xoffset, double yoffset)
{
	mpCamera->Zoom(yoffset);
}

void Program::LightMode(Light* light)
{
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, nearDistance, farDistance);
	glm::mat4 lightView = glm::lookAt(glm::vec3(-10.0f, 10.0f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1.0f, 0));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;


	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	
	// render depth map
	mShaderList[ShaderType::SHADOW_MAP].Use();

	mShaderList[ShaderType::SHADOW_MAP].Set("lightSpaceMatrix",lightSpaceMatrix);

	glm::mat4 trans;
	mShaderList[(int)ShaderType::SHADOW_MAP].Set("transform", trans);

	SceneManager::Instance()->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);

	// normal render
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mShaderList[(int)ShaderType::LIGHT].Use();
	mShaderList[(int)ShaderType::LIGHT].Set("transform", trans);

	glm::mat4 projection = glm::perspective(glm::radians(mpCamera->GetZoom()), width / height, 0.1f, 100.0f);
	mShaderList[(int)ShaderType::LIGHT].Set("projection", projection);

	glm::mat4 view = mpCamera->GetViewMatrix();
	mShaderList[(int)ShaderType::LIGHT].Set("view", view);

	glm::vec3 viewPos = mpCamera->GetPosition();
	mShaderList[(int)ShaderType::LIGHT].Set("viewPos", viewPos);


	mShaderList[ShaderType::LIGHT].Set("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE2);
	mShaderList[ShaderType::LIGHT].Set("shadowMap", (int)mDepthMap);
	glBindTexture(GL_TEXTURE_2D, mDepthMap);
	glActiveTexture(GL_TEXTURE0);

	mShaderList[ShaderType::LIGHT].Set("light", light);
	SceneManager::Instance()->Draw();
}

void Program::DepthTest()
{
	mShaderList[(int)ShaderType::DEPTH].Use();
	glm::mat4 trans;
	mShaderList[(int)ShaderType::DEPTH].Set("transform", trans);

	glm::mat4 projection = glm::perspective(glm::radians(mpCamera->GetZoom()), width / height, 0.1f, 100.0f);
	mShaderList[(int)ShaderType::DEPTH].Set("projection", projection);

	glm::mat4 view = mpCamera->GetViewMatrix();
	mShaderList[(int)ShaderType::DEPTH].Set("view", view);

	glm::vec3 viewPos = mpCamera->GetPosition();
	mShaderList[(int)ShaderType::DEPTH].Set("viewPos", viewPos);


	mShaderList[(int)ShaderType::DEPTH].Set("near", nearDistance);
	mShaderList[(int)ShaderType::DEPTH].Set("far", farDistance);
	SceneManager::Instance()->Draw();

}

void Program::OutlineMode(Light* light)
{
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	mShaderList[(int)ShaderType::LIGHT].Use();

	glm::mat4 trans;
	mShaderList[(int)ShaderType::LIGHT].Set("transform", trans);

	glm::mat4 projection = glm::perspective(glm::radians(mpCamera->GetZoom()), width / height, 0.1f, 100.0f);
	mShaderList[(int)ShaderType::LIGHT].Set("projection", projection);

	glm::mat4 view = mpCamera->GetViewMatrix();
	mShaderList[(int)ShaderType::LIGHT].Set("view", view);

	glm::vec3 viewPos = mpCamera->GetPosition();
	mShaderList[(int)ShaderType::LIGHT].Set("viewPos", viewPos);
	mShaderList[(int)ShaderType::LIGHT].Set("light", light);

	SceneManager::Instance()->Draw();

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	mShaderList[(int)ShaderType::OUTLINE].Use();
	trans = glm::scale(trans, glm::vec3(1.01, 1.01, 1.01));
	mShaderList[(int)ShaderType::OUTLINE].Set("transform", trans);
	mShaderList[(int)ShaderType::OUTLINE].Set("projection", projection);
	mShaderList[(int)ShaderType::OUTLINE].Set("view", view);
	mShaderList[(int)ShaderType::OUTLINE].Set("viewPos", viewPos);

	SceneManager::Instance()->Draw();

	glStencilMask(0xFF);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

}

void Program::FaceCullingMode(Light* light)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// same as below
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CW);
	mShaderList[(int)ShaderType::LIGHT].Use();
	glm::mat4 trans;
	mShaderList[(int)ShaderType::LIGHT].Set("transform", trans);

	glm::mat4 projection = glm::perspective(glm::radians(mpCamera->GetZoom()), width / height, 0.1f, 100.0f);
	mShaderList[(int)ShaderType::LIGHT].Set("projection", projection);

	glm::mat4 view = mpCamera->GetViewMatrix();
	mShaderList[(int)ShaderType::LIGHT].Set("view", view);

	glm::vec3 viewPos = mpCamera->GetPosition();
	mShaderList[(int)ShaderType::LIGHT].Set("viewPos", viewPos);


	mShaderList[(int)ShaderType::LIGHT].Set("light", light);
	SceneManager::Instance()->Draw();

	glCullFace(GL_BACK);
}

void Program::PostProcessMode(Light * light)
{
	static bool firstEnter = true;
	static unsigned int quadVAO, quadVBO;
	static unsigned int FBO, RBO, screenFBO, textureMultiSampled, screenTexture;

	static float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	if (firstEnter)
	{
		firstEnter = false;

		// configure MSAA framebuffer
		// generate frame buffer
		glGenFramebuffers(1, &FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		// generate and bind texture for frame buffer
		glGenTextures(1, &textureMultiSampled);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureMultiSampled);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureMultiSampled, 0);

		// generate and bind render buffer object
		glGenRenderbuffers(1, &RBO);
		// bind render buffer
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// bind quad VAO and VBO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		// configure screen framebuffer
		glGenFramebuffers(1, &screenFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

		glGenTextures(1, &screenTexture);
		glBindTexture(GL_TEXTURE_2D, screenTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	// render objects to frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	LightMode(light);

	// blit multisampled buffer to normal colorbuffer of screenFBO
	glBindFramebuffer(	GL_READ_FRAMEBUFFER, FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screenFBO);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// render frame buffer to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	//clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	mShaderList[ShaderType::POST_PROCESS].Use();
	mShaderList[ShaderType::POST_PROCESS].Set("type", (int)mEffectType);
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
}

void Program::GeometryUsedMode(Light * light)
{
	mShaderList[(int)ShaderType::GEOMETRY].Use();
	glm::mat4 trans;
	mShaderList[(int)ShaderType::GEOMETRY].Set("transform", trans);

	glm::mat4 projection = glm::perspective(glm::radians(mpCamera->GetZoom()), width / height, 0.1f, 100.0f);
	mShaderList[(int)ShaderType::GEOMETRY].Set("projection", projection);

	glm::mat4 view = mpCamera->GetViewMatrix();
	mShaderList[(int)ShaderType::GEOMETRY].Set("view", view);

	glm::vec3 viewPos = mpCamera->GetPosition();
	mShaderList[(int)ShaderType::GEOMETRY].Set("viewPos", viewPos);

	mShaderList[(int)ShaderType::GEOMETRY].Set("time", (float)glfwGetTime());


	mShaderList[(int)ShaderType::GEOMETRY].Set("light", light);
	SceneManager::Instance()->Draw();
}

void Program::InstancingMode(Light * light)
{
	mShaderList[(int)ShaderType::INSTANC].Use();
	glm::mat4 trans;
	mShaderList[(int)ShaderType::INSTANC].Set("transform", trans);

	glm::mat4 projection = glm::perspective(glm::radians(mpCamera->GetZoom()), width / height, 0.1f, 100.0f);
	mShaderList[(int)ShaderType::INSTANC].Set("projection", projection);

	glm::mat4 view = mpCamera->GetViewMatrix();
	mShaderList[(int)ShaderType::INSTANC].Set("view", view);

	glm::vec3 viewPos = mpCamera->GetPosition();
	mShaderList[(int)ShaderType::INSTANC].Set("viewPos", viewPos);


	mShaderList[(int)ShaderType::INSTANC].Set("light", light);
	SceneManager::Instance()->Draw();

}


bool Program::Init()
{
	//initializa GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create window
	mpWindow = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
	if (mpWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(mpWindow);
	
	//initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	//set viewport
	glViewport(0, 0, width, height);

	//set callback functions
	glfwSetFramebufferSizeCallback(mpWindow, Program::FramebufferSizeCallback);
	glfwSetCursorPosCallback(mpWindow, Program::MouseCallBack);
	glfwSetScrollCallback(mpWindow, Program::ScrollCallBack);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	mpCamera = new Camera(glm::vec3(0, 10.0f, 5.0f));

	mSkybox = new Cubemap({
		"Images/skybox/right.jpg",
		"Images/skybox/left.jpg",
		"Images/skybox/top.jpg",
		"Images/skybox/bottom.jpg",
		"Images/skybox/front.jpg",
		"Images/skybox/back.jpg",
	});
	Shader skyboxShader;
	if (!skyboxShader.Create("Shader/Skybox.vert", "Shader/Skybox.frag"))
		return false;
	mSkybox->SetShader(skyboxShader);

	mDeltaTime = 0.0f;
	mLastFrame = 0.0f;

	Shader shader;
	if (!shader.Create("Shader/VertexShader.vert", "Shader/FragShader.frag"))
		return false;
	mShaderList.push_back(shader);

	if (!shader.Create("Shader/DepthTest.vert", "Shader/DepthTest.frag"))
		return false;
	mShaderList.push_back(shader);

	if (!shader.Create("Shader/Outline.vert", "Shader/Outline.frag"))
		return false;
	mShaderList.push_back(shader);

	if (!shader.Create("Shader/PostProcess.vert", "Shader/PostProcess.frag"))
		return false;
	mShaderList.push_back(shader);

	if (!shader.Create("Shader/Geometry.vert", "Shader/Geometry.frag", "Shader/Geometry.geom"))
		return false;
	mShaderList.push_back(shader);

	if (!shader.Create("Shader/Instancing.vert", "Shader/Instancing.frag"))
		return false;
	mShaderList.push_back(shader);

	if (!shader.Create("Shader/ShadowMap.vert", "Shader/ShadowMap.frag"))
		return false;
	mShaderList.push_back(shader);

	cntModeIndex = 0;

	// add model
	Model* model = new Model("Model/nanosuit.obj", "Nanosuit");
	model->SetShader(mShaderList[cntModeIndex]);
	SceneManager::Instance()->AddModel(model);

	// depth map frame
	glGenFramebuffers(1, &mDepthMapFBO);

	// depth map
	glGenTextures(1, &mDepthMap);
	glBindTexture(GL_TEXTURE_2D, mDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);

	// bind
	glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void Program::Run()
{
	DirectionalLight directionalLight(glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, -1.0f, 0));
	PointLight pointLight(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0, 10.0f, 5.0f), 1.0f, 0.09f, 0.032f);
	SpotLight spotLight(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0, 0, -1.0f), glm::vec3(0, 10.0f, 3.0f),  1.0f, 0.09f, 0.032f, glm::cos(glm::radians(15.0f)), glm::cos(glm::radians(25.0f)));


	while (!glfwWindowShouldClose(mpWindow))
	{
		//pre frame
		float currentFrame = glfwGetTime();
		mDeltaTime = currentFrame - mLastFrame;
		mLastFrame = currentFrame;

		//input
		ProcessInput(mpWindow);

		//render parameters
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		mSkybox->Draw();		

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// draw
		switch (cntModeIndex)
		{
		case 0:
			LightMode(&directionalLight);
			break;
		case 1:
			DepthTest();
			break;
		case 2:
			OutlineMode(&pointLight);
			break;
		case 3:
			FaceCullingMode(&directionalLight);
			break;
		case 4:
			PostProcessMode(&spotLight);
			break;
		case 5:
			GeometryUsedMode(&directionalLight);
			break;
		case 6:
			InstancingMode(&directionalLight);
			break;
		default:
			break;
		}


		//check events and swap buffers
		glfwPollEvents();
		glfwSwapBuffers(mpWindow);
	}


}

void Program::Destroy()
{
	glfwTerminate();

	SceneManager::Instance()->Destroy();

	delete mpCamera;
}

void Program::ProcessInput(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		mpCamera->Move(Camera::FORWARD, mDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		mpCamera->Move(Camera::BACKWARD, mDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		mpCamera->Move(Camera::LEFT, mDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		mpCamera->Move(Camera::RIGHT, mDeltaTime);

	// change shader
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS  && cntModeIndex != 0)
	{
		std::cout << "Change to Light Shader" << std::endl;
		cntModeIndex = 0;
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS   && cntModeIndex != 1)
	{
		std::cout << "Change to Depth Testing" << std::endl;
		cntModeIndex = 1;
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS  && cntModeIndex != 2)
	{
		std::cout << "Draw Outline" << std::endl;
		cntModeIndex = 2;
	}
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS  && cntModeIndex != 3)
	{
		std::cout << "Face Culling" << std::endl;
		cntModeIndex = 3;
	}
	else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && cntModeIndex != 4)
	{
		std::cout << "Post Processing with MSAA" << std::endl;
		cntModeIndex = 4;
	}
	else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && cntModeIndex != 5)
	{
		std::cout << "Use Geometry Shader" << std::endl;
		cntModeIndex = 5;
	}
	else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && cntModeIndex != 6)
	{
		std::cout << "Instancing" << std::endl;
		cntModeIndex = 6;
	}
	else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && mShaderList.size() > 7)
		cntModeIndex = 7;
	else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && mShaderList.size() > 8)
		cntModeIndex = 8;
	else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && mShaderList.size() > 9)
		cntModeIndex = 9;

	// change post process effect
	if (cntModeIndex == 4)
	{
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			mEffectType = 1;
		else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			mEffectType = 0;
	}
}

