#pragma once

#include <glm\glm.hpp>
#include <action.h>

class WorldObject;

class RotateTowards : public Action
{
public:
	RotateTowards(WorldObject* associatedObject, glm::float32 pitch, glm::float32 yaw, 
		glm::float32 roll, glm::float32 interval);

	void update(glm::float32 deltaTime) override;

private:
	glm::float32 m_pitch;
	glm::float32 m_yaw;
	glm::float32 m_roll;

	glm::float32 m_interval;
	glm::float32 m_totalTime;
};