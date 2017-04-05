#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>

class Transform
{
public:
	Transform();

	glm::quat rotation();
	glm::vec3 euler();
	glm::vec3 forward();
	glm::vec3 up();

	glm::vec3 position;
	glm::vec3 scale;	
	glm::float32 pitch;
	glm::float32 yaw;
	glm::float32 roll;	
};