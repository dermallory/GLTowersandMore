#include "worldobject.h"

WorldObject::WorldObject(Transform transform, model_type modelType)
:
m_modelType(modelType),
m_transform(transform)
{
}

void WorldObject::translateBy(glm::vec3 translation)
{
}

void WorldObject::rotateBy(glm::vec3 rotation)
{
}

void WorldObject::scaleBy(glm::vec3 scale)
{
}

void WorldObject::setPosition(glm::vec3 position)
{
}

void WorldObject::setRotation(glm::vec3 rotation)
{
}

void WorldObject::setScale(glm::vec3 scale)
{
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

const Transform& WorldObject::getTransform() const
{
	return m_transform;
}
