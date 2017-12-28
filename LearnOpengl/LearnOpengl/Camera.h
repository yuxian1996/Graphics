#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <math.h>

#pragma once

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVTY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
	enum Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		IDLE
	} mMovement;

	Camera();
	Camera(const glm::vec3& iPosition = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& iUp = glm::vec3(0.0f, 1.0f, 0.0f), int iYaw = YAW, int iPitch = PITCH);
	~Camera();

	void Move(const Movement iMovement, float iDeltaTime);
	void Rotate(float iXoffset, float iYoffset, bool constrainPitch = true);
	void Zoom(float iYoffset);

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(mPosition, mPosition + mFront, mUp);
	}

	const float GetZoom() { return mZoom; };
	const glm::vec3& GetPosition(){ return mPosition; };
	const glm::vec3& GetDirection() { return mFront; };

private:
	void Update();

	glm::vec3 mPosition;
	glm::vec3 mUp;
	glm::vec3 mRight;
	glm::vec3 mFront;
	glm::vec3 mWorldUp;
	
	//euler angles
	float mYaw;
	float mPitch;
	
	//parameters
	float mSpeed;
	float mMouseSens;
	float mZoom;

};