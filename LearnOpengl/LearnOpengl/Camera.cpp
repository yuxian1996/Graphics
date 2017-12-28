#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(const glm::vec3 & iPosition, const glm::vec3 & iUp, int iYaw, int iPitch):
mPosition(iPosition), mWorldUp(iUp), mYaw(iYaw), mPitch(iPitch), mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mSpeed(SPEED), mMouseSens(SENSITIVTY), mZoom(ZOOM)
{
	Update();
}

Camera::~Camera()
{
}

void Camera::Move(const Movement iMovement, float iDeltaTime)
{
	float velocity = mSpeed * iDeltaTime;
	switch (iMovement)
	{
	case FORWARD:
		mPosition += mFront * velocity;
		break;
	case BACKWARD:
		mPosition -= mFront * velocity;
		break;
	case LEFT:
		mPosition -= mRight * velocity;
		break;
	case RIGHT:
		mPosition += mRight * velocity;
		break;
	default:
		break;
	}
	Update();
}

void Camera::Rotate(float iXoffset, float iYoffset, bool iConstrainPitch)
{
	iXoffset *= mMouseSens;
	iYoffset *= mMouseSens;

	mYaw += iXoffset;
	mPitch += iYoffset;

	if (iConstrainPitch)
	{
		if (mPitch > 89.0f)
			mPitch = 89.0f;
		if (mPitch < -89.0f)
			mPitch = -89.0f;
	}

	Update();
}

void Camera::Zoom(float iYoffset)
{
	if (mZoom >= 1.0f && mZoom <= 45.0f)
		mZoom -= iYoffset;
	if (mZoom <= 1.0f)
		mZoom = 1.0f;
	if (mZoom >= 45.0f)
		mZoom = 45.0f;

	Update();
}

void Camera::Update()
{
	glm::vec3 front;
	front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	front.y = sin(glm::radians(mPitch));
	front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));

	mFront = glm::normalize(front);
	mRight = glm::normalize(glm::cross(mFront, mWorldUp));
	mUp = glm::normalize(glm::cross(mRight, mFront));
}
