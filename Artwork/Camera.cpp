
#include "Camera.h"
#include <glm\ext\vector_float3.hpp>
#include <glm\ext\matrix_clip_space.hpp>
#include <glm\ext\matrix_float4x4.hpp>
#include <glm\ext\matrix_transform.hpp>
#include <GLFW\glfw3.h>
#include "../Input/Input.h"
Camera::Camera(CAMERA_TYPE type, glm::vec3 position) {
	m_Type = PERSPECTIVE;
	Translation = position;
	cameraDirection = glm::vec3(0, 0, 1);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);
}

Camera::Camera() {
	m_Type = PERSPECTIVE;
	Translation = glm::vec3(2, 1, 0.5);
	cameraDirection = glm::vec3(0, 0, 1);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);
}

void Camera::SetType(CAMERA_TYPE type)
{
	m_Type = type;
}


void Camera::Update(Timestep ts) {
	float x = Input::GetMouseX();
	float y = Input::GetMouseY();

	if (!controlled && Input::IsMouseButtonPressed(1))
	{
		mouseLastX = x;
		mouseLastY = y;
	}

	controlled = Input::IsMouseButtonPressed(1);

	if (!controlled)
		Input::ShowMouse();
	else
		Input::HideMouse();

	if (Input::IsKeyPress(GLFW_KEY_UP))
		Speed += 0.01f;
	else if (Input::IsKeyPress(GLFW_KEY_DOWN))
		Speed -= 0.01f;
	if (Speed < 0)
		Speed = 0;

	// Keyboard
	if (!controlled) {
		return;
	}

	if (m_Type == CAMERA_TYPE::ORTHO) {
		if (Input::IsKeyPressed(GLFW_KEY_RIGHT))
			Translation.x += Speed * ts;
		if (Input::IsKeyPressed(GLFW_KEY_LEFT))
			Translation.x -= Speed * ts;

		if (Input::IsKeyPressed(GLFW_KEY_UP))
			Translation.y += Speed * ts;
		if (Input::IsKeyPressed(GLFW_KEY_DOWN))
			Translation.y -= Speed * ts;
	}
	else {
		glm::vec3 movement = glm::vec3(0, 0, 0);

		if (Input::IsKeyPressed(GLFW_KEY_D))
			movement -= cameraRight * (Speed * ts);
		if (Input::IsKeyPressed(GLFW_KEY_A))
			movement += cameraRight * (Speed * ts);

		if (Input::IsKeyPressed(GLFW_KEY_W))
			movement += cameraDirection * (Speed * ts);
		if (Input::IsKeyPressed(GLFW_KEY_S))
			movement -= cameraDirection * (Speed * ts);
		if (Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
			movement -= up * (Speed * ts);
		if (Input::IsKeyPressed(GLFW_KEY_SPACE))
			movement += up * (Speed * ts);


		Translation += glm::vec3(movement.x, 0, 0);

		Translation += glm::vec3(0, movement.y, 0);

		Translation += glm::vec3(0, 0, movement.z);
	}


	if (firstMouse)
	{
		mouseLastX = x;
		mouseLastY = y;
		firstMouse = false;
	}
	// mouse


	float diffx = x - mouseLastX;
	float diffy = mouseLastY - y;
	mouseLastX = x;
	mouseLastY = y;

	const float sensitivity = 0.1f;
	diffx *= sensitivity;
	diffy *= sensitivity;

	Yaw += diffx;
	Pitch += diffy;

	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;

	cameraDirection.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	cameraDirection.y = sin(glm::radians(Pitch));
	cameraDirection.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	cameraFront = glm::normalize(cameraDirection);
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
}

void Camera::OnWindowResize(int x, int y)
{
	AspectRatio = (float)x / (float)y;
	float width = y * (16 * 9);
	float height = y;

}

glm::vec3 Camera::GetTranslation() {
	return Translation;
}

glm::vec3 Camera::GetDirection() {
	return cameraDirection;
}

glm::mat4 Camera::GetPerspective()
{

	m_Perspective = glm::perspectiveFov(glm::radians(Fov), 9.0f * AspectRatio, 9.0f, 0.1f, 2000.0f);
	//m_Perspective = glm::ortho(-8.0f, 8.0f, -4.5f, 4.5f, -1.0f, 1.0f);
	return m_Perspective;
}

glm::mat4 Camera::GetTransform()
{
	glm::mat4 tr = lookAt(Translation, Translation + cameraFront, cameraUp);
	return tr;

}

glm::vec3 Camera::GetLookAt()
{
	return Translation + cameraFront;
}



