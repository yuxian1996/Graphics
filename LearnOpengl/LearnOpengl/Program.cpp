#include "Program.h"

Camera* Program::mpCamera = NULL;

//Callback
void Program::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Program::MouseCallBack(GLFWwindow * window, double xpos, double ypos)
{
	static float lastX = 800 / 2.0f;
	static float lastY = 600 / 2.0f;
	static bool firstMouse = false;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		//check raycast
		HitInfo info;
		//Utility::Raycast(glm::vec3(0.2f, 2, 5), glm::vec3(0,0,-1), 1000, info);

		float upAngle = (300.0 - ypos) / 300.0f * 22.5f;
		float rightAngle = (400.0f - xpos) / 400.0f * (22.5f * 800 / 600);
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
	mpWindow = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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
	glViewport(0, 0, 800, 600);

	//set callback functions
	glfwSetFramebufferSizeCallback(mpWindow, Program::FramebufferSizeCallback);
	glfwSetCursorPosCallback(mpWindow, Program::MouseCallBack);
	glfwSetScrollCallback(mpWindow, Program::ScrollCallBack);

	glEnable(GL_DEPTH_TEST);

	mpCamera = new Camera(glm::vec3(0.2f, 0.2f, 5.0f));
	mDeltaTime = 0.0f;
	mLastFrame = 0.0f;


	return true;
}

void Program::Run()
{
	Shader shader;
	if (!shader.Create("Shader/VertexShader.vert", "Shader/FragShader.frag"))
		return;

	Light directionLight = { LightType::DIRECTIONALLIGHT, glm::vec3(0,0,0), glm::vec3(0, 0, -1.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8f, 0.8f, 0.8f) };
	Light pointLight = { LightType::POINTLIGHT, glm::vec3(0, 10.0f, 5.0f), glm::vec3(0, 0, 0), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8f, 0.8f, 0.8f), 1.0f, 0.09f, 0.032f };
	Light spotLight = { LightType::SPOTLIGHT, glm::vec3(0, 10.0f, 3.0f), glm::vec3(0, 0, -1.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8f, 0.8f, 0.8f), 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(15.0f)), glm::cos(glm::radians(25.0f)) };

	Model* model = new Model("Model/nanosuit.obj");
	//Model* model = new Model("Model/Cube/cube.obj");
	model->SetShader(shader);
	SceneManager::Instance()->AddModel(model);

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//draw
		shader.Use();

		glm::mat4 trans;
		shader.Set("transform", trans);

		glm::mat4 projection = glm::perspective(glm::radians(mpCamera->GetZoom()), 800.0f / 600.0f, 0.1f, 100.0f);
		shader.Set("projection", projection);

		glm::mat4 view = mpCamera->GetViewMatrix();
		shader.Set("view", view);

		glm::vec3 viewPos = mpCamera->GetPosition();
		shader.Set("viewPos", viewPos);

		shader.Set("light", spotLight);

		SceneManager::Instance()->Draw();


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
