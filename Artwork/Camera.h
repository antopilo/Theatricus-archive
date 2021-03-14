#include <glm/ext/vector_float3.hpp>
#include "glm/mat4x4.hpp"

#include "../Core/Timestep.h"

enum CAMERA_TYPE {
	ORTHO,
	PERSPECTIVE
};

class Camera
{
private:
	CAMERA_TYPE m_Type;

	float AspectRatio = 16.0f / 9.0f;

	glm::vec3 Translation = { 2.0f, 1.0f, 0.5f };
	glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	glm::mat4 m_Perspective;

	glm::vec3 up; // = glm::vec3(0.0f, 1.0f, 0.0f);
	
	
	glm::vec3 cameraFront; // = glm::vec3(0.0f, 0.0f, 1.0f);

	glm::vec3 cameraTarget; // = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDirection; // = glm::normalize(Translation - cameraTarget);


	bool firstMouse = true;

	float Yaw = 0;
	float Pitch = 0;
	float mouseLastX = 0;
	float mouseLastY = 0;

public:
	float Fov = 88.0f;
	float Exposure = 1.0f;
	float Speed = 0.1f;
	glm::vec3 cameraUp; // = glm::cross(cameraDirection, cameraRight);
	glm::vec3 cameraRight; // = glm::normalize(glm::cross(up, cameraDirection));
	Camera();
	Camera(CAMERA_TYPE type, glm::vec3 position);

	void SetType(CAMERA_TYPE type);
	void Update(Timestep ts);
	void OnWindowResize(int x, int y);

	bool controlled = true;
	glm::vec3 GetTranslation();
	glm::vec3 GetDirection();
	glm::mat4 GetPerspective();
	glm::mat4 GetTransform();
	glm::vec3 GetLookAt();

	bool isFocus = false;
};