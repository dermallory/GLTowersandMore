#pragma once

#include <glm\glm.hpp>
#include <action.h>

class WorldObject;

class MoveTowards : public Action
{
public:
	MoveTowards(WorldObject* associatedObject, glm::vec3 positionTo, glm::float32 interval);

	void update(glm::float32 deltaTime) override;

private:
	glm::vec3 m_positionTo;
	glm::float32 m_interval;

	glm::float32 m_totalTime;
};