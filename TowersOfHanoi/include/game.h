#pragma once

#include <glm\glm.hpp>
#include <vector>
#include <stack>

class WorldObject;

enum TopRingState
{
	NONE, BOTTOM, MIDDLE, TOP, RELEASED
};

struct Tower
{
	std::stack<WorldObject*> rings;
	WorldObject* peg;

	glm::float32 bottomHeight;
	glm::float32 topHeight;

	glm::float32 releaseHeight;

	TopRingState topRingState;
};

enum MoveDirection
{
	LEFT, RIGHT, UP, DOWN
};


class Game
{
public:
	Game();
	~Game();

	void update(float deltaTime, int keyPressed);

	void moveRingUpDown(MoveDirection direction);
	void selectTower(Tower* tower, bool moveRing);
	void clearSelections();

	void moveLeft();
	void moveRight();

	TopRingState getTopRingState(Tower* tower);
	bool checkBounds(MoveDirection direction);

public:
	Tower* m_selectedTower;
	std::vector<WorldObject*> m_worldObjects;
	float m_defaultAlpha;
	float m_selectedAlpha;

private:
	float canMoveDelay = 0.0f;

	Tower* m_tower1;
	Tower* m_tower2;
	Tower* m_tower3;
};