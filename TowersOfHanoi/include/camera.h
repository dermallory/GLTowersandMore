#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <transform.h>

struct PerspectiveCameraSettings
{
	PerspectiveCameraSettings(glm::float32 aspect, glm::float32 fov = 45.0f, glm::float32 zNear = 0.1f, 
		glm::float32 zFar = 100.0f, glm::float32 initialZoom = 0.0f)
	:
		m_aspectRatio(aspect),
		m_fov(fov),
		m_zNear(zNear),
		m_zFar(zFar),
		m_zoom(initialZoom)
	{
	}

	glm::float32 m_aspectRatio;
	glm::float32 m_fov;
	glm::float32 m_zNear;
	glm::float32 m_zFar;
	glm::float32 m_zoom;
};

// TODO: 
struct OrthoCameraSettings
{
	OrthoCameraSettings(glm::float32 _left = -1, glm::float32 _right=1, glm::float32 _bottom=-1, glm::float32 _top=1, 
		glm::float32 _zNear=0.1f, glm::float32 _zFar=1.0f)
	:
		left(_left),
		right(_right),
		bottom(_bottom),
		top(_top),
		zNear(_zNear),
		zFar(_zFar)
	{
	}

	glm::float32 left;
	glm::float32 right;
	glm::float32 bottom;
	glm::float32 top;
	glm::float32 zFar;
	glm::float32 zNear;
};

class Camera
{
public:
	Camera() = delete;
	void operator =(const Camera&) = delete;
	Camera(const Camera&) = delete;

public:
	Camera(Transform transform);

	void translateBy(glm::vec3 translation);
	void rotateBy(glm::vec3 rotation);
	void translateBy(glm::float32 x, glm::float32 y, glm::float32 z);
	void rotateBy(glm::float32 x, glm::float32 y, glm::float32 z);

	void setPosition(glm::vec3 position);
	void setRotation(glm::vec3 rotation);

	Transform& transform();
	
	virtual glm::mat4 viewProjection() = 0;

protected:
	Transform m_transform;
};


class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(Transform transform, PerspectiveCameraSettings settings);
	glm::mat4 viewProjection() override;

	PerspectiveCameraSettings m_settings;
};

class OrthoCamera : public Camera
{
public:
	OrthoCamera(Transform transform, OrthoCameraSettings settings);
	glm::mat4 viewProjection() override;

	OrthoCameraSettings m_settings;
};
