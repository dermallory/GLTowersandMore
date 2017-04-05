#pragma once

#include <glm\glm.hpp>

struct Camera
{
	glm::vec3 position;
	glm::vec3 rotation;

	glm::float32 zoom;
	glm::float32 fov;
	glm::float32 zNear;
	glm::float32 zFar;
};
