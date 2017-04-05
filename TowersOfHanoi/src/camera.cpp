
#include <camera.h>
#include <glm\gtc\matrix_transform.hpp>

Camera::Camera(Transform transform)
:
m_transform(transform)
{
}

void Camera::translateBy(glm::vec3 translation)
{
	m_transform.position += m_transform.rotation()*translation;
}

void Camera::rotateBy(glm::vec3 rotation)
{
	m_transform.pitch += rotation.x;
	m_transform.yaw += rotation.y;
	m_transform.roll += rotation.z;
}

void Camera::translateBy(glm::float32 x, glm::float32 y, glm::float32 z)
{
	m_transform.position.x += x;
	m_transform.position.y += y;
	m_transform.position.z += z;	
}

void Camera::rotateBy(glm::float32 x, glm::float32 y, glm::float32 z)
{
	m_transform.pitch += x;
	m_transform.yaw += y;
	m_transform.roll += z;
}

void Camera::setPosition(glm::vec3 position)
{
	m_transform.position = position;
}

void Camera::setRotation(glm::vec3 rotation)
{
	m_transform.pitch = rotation.x;
	m_transform.yaw = rotation.y;
	m_transform.roll = rotation.z;
}

Transform& Camera::transform()
{
	return m_transform;
}

// perspective
PerspectiveCamera::PerspectiveCamera(Transform transform, PerspectiveCameraSettings settings)
:
Camera(transform),
m_settings(settings)
{
}

glm::mat4 PerspectiveCamera::viewProjection()
{
	glm::mat4 camera_zoom = glm::scale(glm::mat4(), glm::vec3(m_settings.m_zoom+1.0f, m_settings.m_zoom+1.0f, m_settings.m_zoom+1.0f));
	glm::vec3 cameraForward = m_transform.forward();
	glm::vec3 cameraUp = m_transform.up();

	glm::mat4 view_mat = glm::lookAt(m_transform.position, m_transform.position + cameraForward, cameraUp)*camera_zoom;
	glm::mat4 projection_mat = glm::perspective(m_settings.m_fov, m_settings.m_aspectRatio, m_settings.m_zNear, m_settings.m_zFar);

	return projection_mat*view_mat;
}


// orthographic
OrthoCamera::OrthoCamera(Transform transform, OrthoCameraSettings settings)
:
Camera(transform),
m_settings(settings)
{
}

glm::mat4 OrthoCamera::viewProjection()
{
	glm::vec3 cameraForward = m_transform.forward();
	glm::vec3 cameraUp = m_transform.up();

	glm::mat4 view_mat = glm::lookAt(m_transform.position, m_transform.position + cameraForward, cameraUp);
	glm::mat4 projection_mat = glm::ortho(m_settings.left, m_settings.right, m_settings.bottom, m_settings.top, m_settings.zNear, m_settings.zFar);

	return projection_mat*view_mat;
}
