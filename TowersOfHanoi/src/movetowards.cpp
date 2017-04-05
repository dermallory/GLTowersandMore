#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <movetowards.h>
#include <worldobject.h>


MoveTowards::MoveTowards(WorldObject* associatedObject, glm::vec3 positionTo, glm::float32 interval)
:
	Action(associatedObject),
	m_positionTo(positionTo),
	m_interval(interval),
	m_totalTime(0)
{
}

void MoveTowards::update(glm::float32 deltaTime)
{
	if (!m_completed && m_totalTime >= m_interval)
	{
		Transform& transform = m_associatedObject->getTransform();
		transform.position = m_positionTo;

		m_completed = true;
		return;
	}

	// translate the object
	if (!m_completed)
	{
		Transform& transform = m_associatedObject->getTransform();

		glm::vec3 direction = glm::normalize(m_positionTo - transform.position);
		glm::float32 distance = glm::distance(transform.position, m_positionTo);

		glm::float32 changeInTime = m_interval - m_totalTime;

		glm::float32 speed = distance/glm::clamp(changeInTime, glm::epsilon<glm::float32>(), distance);

		transform.position += direction*speed*deltaTime;
	}

	m_totalTime += deltaTime;
}
