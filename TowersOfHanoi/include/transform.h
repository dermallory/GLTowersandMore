#pragma once

#include <glm\glm.hpp>

struct Transform
{
	glm::quat rotation()
	{
		glm::quat x = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
		glm::quat y = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
		glm::quat z = glm::angleAxis(roll, glm::vec3(0, 0, -1));

		glm::quat outputRot = glm::normalize(x*y*z);

		return outputRot;
	}

	glm::vec3 euler()
	{
		return glm::eulerAngles(rotation());
	}

	glm::vec3 forward()
	{
		glm::quat outputRot = rotation();

		return outputRot*glm::vec3(0, 0, -1);
	}

	glm::vec3 up()
	{
		glm::quat outputRot = rotation();

		return outputRot*glm::vec3(0, 1, 0);
	}

	glm::vec3 position;
	glm::vec3 scale;	
	glm::float32 pitch;
	glm::float32 yaw;
	glm::float32 roll;	
};