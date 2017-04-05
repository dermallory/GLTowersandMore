#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <rotatetowards.h>
#include <worldobject.h>


RotateTowards::RotateTowards(WorldObject* associatedObject, glm::float32 pitch, glm::float32 yaw, 
	glm::float32 roll, glm::float32 interval)
:
	Action(associatedObject),
	m_pitch(pitch),
	m_yaw(yaw),
	m_roll(roll),
	m_interval(interval),
	m_totalTime(0)
{
}

void RotateTowards::update(glm::float32 deltaTime)
{
	if (!m_completed && m_totalTime >= m_interval)
	{
		Transform& transform = m_associatedObject->getTransform();
		transform.pitch = m_pitch;
		transform.yaw = m_yaw;
		transform.roll = m_roll;

		m_completed = true;
		return;
	}

	// translate the object
	if (!m_completed)
	{
		Transform& transform = m_associatedObject->getTransform();

		glm::float32 changeInPitch = m_pitch - transform.pitch;
		glm::float32 changeInYaw = m_yaw - transform.yaw;
		glm::float32 changeInRoll = m_roll - transform.roll;

		glm::float32 changeInTime = m_interval - m_totalTime;

		glm::float32 speedPitch = changeInPitch/glm::clamp(changeInTime, glm::epsilon<glm::float32>(), changeInPitch);
		glm::float32 speedYaw = changeInYaw/glm::clamp(changeInTime, glm::epsilon<glm::float32>(), changeInYaw);
		glm::float32 speedRoll = changeInRoll/glm::clamp(changeInTime, glm::epsilon<glm::float32>(), changeInRoll);

		transform.pitch += speedPitch*deltaTime;
		transform.yaw += speedYaw*deltaTime;
		transform.roll += speedRoll*deltaTime;
	}

	m_totalTime += deltaTime;
}
