#pragma once

#include <glm\glm.hpp>
#include <transform.h>

enum CameraProjection
{
	PERSPECTIVE, ORTHOGRAPHIC
};

class Camera
{
public:
	Camera() = delete;
	void operator =(const Camera&) = delete;
	Camera(const Camera&) = delete;

public:
	Camera(Transform transform, glm::float32 zoom, glm::float32 fov, glm::float32 zNear, glm::float32 zFar, glm::float32 aspect);
	Camera(Transform transform, glm::float32 zoom, glm::float32 fov, glm::float32 zNear, glm::float32 zFar, glm::float32 aspect);

	void test()
	{
		
	}

	void translateBy(glm::vec3 translation);
	void rotateBy(glm::vec3 rotation);
	void scaleBy(glm::vec3 scale);

	void setPosition(glm::vec3 position);
	void setRotation(glm::vec3 rotation);
	void setScale(glm::vec3 scale);

	glm::mat4 viewProjection();

private:
	Transform m_transform;

	glm::float32 m_zoom;
	glm::float32 m_fov;
	glm::float32 m_zNear;
	glm::float32 m_zFar;
};
