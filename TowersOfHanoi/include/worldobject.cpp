#include "worldobject.h"
#include <action.h>

WorldObject::WorldObject(Transform transform, model_type modelType)
:
	m_modelType(modelType),
	m_transform(transform),
	m_color(),
	m_actions()
{
}

void WorldObject::update(glm::float32 deltaTime)
{
	for each(auto action in m_actions)
	{
		if (action->isComplete())
		{
			delete action;
		}
		else
		{
			action->update(deltaTime);
		}
	}

	m_actions.remove_if([](Action* action){ return !action || action->isComplete(); });
}

void WorldObject::translateBy(glm::vec3 translation)
{
	m_transform.position += translation;
}

void WorldObject::rotateBy(glm::vec3 rotation)
{
	m_transform.pitch += rotation.x;
	m_transform.yaw += rotation.y;
	m_transform.roll += rotation.z;
}

void WorldObject::scaleBy(glm::vec3 scale)
{
	m_transform.scale += scale;
}

void WorldObject::translateBy(glm::float32 x, glm::float32 y, glm::float32 z)
{
	m_transform.position.x += x;
	m_transform.position.y += y;
	m_transform.position.z += z;	
}

void WorldObject::rotateBy(glm::float32 x, glm::float32 y, glm::float32 z)
{
	m_transform.pitch += x;
	m_transform.yaw += y;
	m_transform.roll += z;
}

void WorldObject::scaleBy(glm::float32 x, glm::float32 y, glm::float32 z)
{
	m_transform.scale.x += x;
	m_transform.scale.y += y;
	m_transform.scale.z += z;	
}

void WorldObject::setPosition(glm::vec3 position)
{
	m_transform.position = position;
}

void WorldObject::setRotation(glm::vec3 rotation)
{
	m_transform.pitch = rotation.x;
	m_transform.yaw = rotation.y;
	m_transform.roll = rotation.z;
}

void WorldObject::setScale(glm::vec3 scale)
{
	m_transform.scale = scale;
}

void WorldObject::pushAction(Action* action)
{
	m_actions.push_back(action);
}

void WorldObject::setColor(glm::vec4 color)
{
	m_color = color;
}

glm::vec4 WorldObject::getColor() const
{
	return m_color;
}

model_type WorldObject::getModelType() const
{
	return m_modelType;
}

Transform WorldObject::getTransform()
{
	return m_transform;
}
