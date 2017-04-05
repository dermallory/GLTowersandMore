#pragma once

class WorldObject;

class Action
{
public:
	Action(WorldObject* associatedObject);

	virtual void update(glm::float32 deltaTime) = 0;

	bool isComplete() const;

protected:
	WorldObject* m_associatedObject;
	bool m_completed;
};