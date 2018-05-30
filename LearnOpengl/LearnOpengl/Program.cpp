#include "Program.h"

Camera* Program::mpCamera = NULL;
float Program::width = 800, Program::height = 600, Program::farDistance = 100.0f, Program::nearDistance = 0.1f;;

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

		float upAngle = (height / 2.0f - ypos) / height / 2.0f * 22.5f;
		float rightAngle = (width / 2.0f - xpos) / width / 2.0f * (22.5f * width / height);
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

	cntShaderIndex = 0;

	return true;
}

void Program::Run()
{
	DirectionalLight directionalLight(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0, 0, -1.0f));
	PointLight pointLight(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0, 10.0f, 5.0f), 1.0f, 0.09f, 0.032f);
	SpotLight spotLight(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0, 0, -1.0f), glm::vec3(0, 10.0f, 3.0f),  1.0f, 0.09f, 0.032f, glm::cos(glm::radians(15.0f)), glm::cos(glm::radians(25.0f)));

	//Model* model = new Model("Model/Room/room.obj", "Room");
	//model->SetShader(shader);
	//SceneManager::Instance()->AddModel(model);

	Model* model = new Model("Model/nanosuit.obj", "Nanosuit");
	model->SetShader(mShaderList[cntShaderIndex]);
	SceneManager::Instance()->AddModel(model);

	//SceneManager::Instance()->DestroyModel(model);

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
		mShaderList[cntShaderIndex].Use();
		glm::mat4 trans;
		mShaderList[cntShaderIndex].Set("transform", trans);

		glm::mat4 projection = glm::perspective(glm::radians(mpCamera->GetZoom()), width / height, 0.1f, 100.0f);
		mShaderList[cntShaderIndex].Set("projection", projection);

		glm::mat4 view = mpCamera->GetViewMatrix();
		mShaderList[cntShaderIndex].Set("view", view);

		glm::vec3 viewPos = mpCamera->GetPosition();
		mShaderList[cntShaderIndex].Set("viewPos", viewPos);


		//draw
		switch (cntShaderIndex)
		{
		case 0 :
			mShaderList[cntShaderIndex].Set("light", &spotLight);
			SceneManager::Instance()->Draw();
			break;
		case 1:
			mShaderList[cntShaderIndex].Set("near", nearDistance);
			mShaderList[cntShaderIndex].Set("far", farDistance);
			SceneManager::Instance()->Draw();
			break;
		case 2:
		{
			glEnable(GL_STENCIL_TEST);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			mShaderList[0].Use();
			mShaderList[cntShaderIndex].Set("transform", trans);
			mShaderList[cntShaderIndex].Set("projection", projection);
			mShaderList[cntShaderIndex].Set("view", view);
			mShaderList[cntShaderIndex].Set("viewPos", viewPos);

			SceneManager::Instance()->Draw();

			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);
			mShaderList[cntShaderIndex].Use();
			trans = glm::scale(trans, glm::vec3(1.01, 1.01, 1.01));
			mShaderList[cntShaderIndex].Set("transform", trans);
			mShaderList[cntShaderIndex].Set("projection", projection);
			mShaderList[cntShaderIndex].Set("view", view);
			mShaderList[cntShaderIndex].Set("viewPos", viewPos);

			SceneManager::Instance()->Draw();

			glStencilMask(0xFF);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_STENCIL_TEST);
		}
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
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && mShaderList.size() > 0 && cntShaderIndex != 0)
	{
		std::cout << "Change to Light Shader" << std::endl;
		cntShaderIndex = 0;
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS  && mShaderList.size() > 1 && cntShaderIndex != 1)
	{
		std::cout << "Change to Depth Testing" << std::endl;
		cntShaderIndex = 1;
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && mShaderList.size() > 2 && cntShaderIndex != 2)
	{
		std::cout << "Draw Outline" << std::endl;
		cntShaderIndex = 2;
	}
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && mShaderList.size() > 3)
		cntShaderIndex = 3;
	else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && mShaderList.size() > 4)
		cntShaderIndex = 4;
	else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && mShaderList.size() > 5)
		cntShaderIndex = 5;
	else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && mShaderList.size() > 6)
		cntShaderIndex = 6;
	else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && mShaderList.size() > 7)
		cntShaderIndex = 7;
	else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && mShaderList.size() > 8)
		cntShaderIndex = 8;
	else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && mShaderList.size() > 9)
		cntShaderIndex = 9;
}

void Program::BindBuffer()
{
	float vertices[] = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	//generate
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);

	//bind 
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//set vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//unbind buffer and array
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}
