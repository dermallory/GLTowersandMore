#pragma once

#include <model.h>
#include <transform.h>

// Treated like an instance of a "model".
//
// each frame: 
//			update transform,
//			swap for the appropriate vao stored by the model,
//			grab the color,
//			grab the mesh,
//			set the shader,
//			render
class WorldObject
{
public:
	WorldObject() = delete;
	WorldObject(const WorldObject&) = delete;

public:
	WorldObject(Transform transform, model_type type);
	
	void translateBy(glm::vec3 translation);
	void rotateBy(glm::vec3 rotation);
	void scaleBy(glm::vec3 scale);

	void setPosition(glm::vec3 position);
	void setRotation(glm::vec3 rotation);
	void setScale(glm::vec3 scale);

	void setColor(glm::vec4 color);
	glm::vec4 WorldObject::getColor() const;

	model_type getModelType() const;
	const Transform& getTransform() const;

private:
	Transform m_transform;
	model_type m_modelType;

	glm::vec4 m_color;
};