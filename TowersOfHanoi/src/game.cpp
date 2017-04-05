#include <game.h>
#include <worldobject.h>
#include <glfw\glfw3.h>

#include <random>
#include <ctime>

Game::Game()
:
m_worldObjects(),
m_selectedTower(nullptr),
m_tower1(nullptr),
m_tower2(nullptr),
m_tower3(nullptr),
m_defaultAlpha(0.2f),
m_selectedAlpha(1.0f)
{
	srand(static_cast<unsigned int>(time(NULL)));
	
	// towers
	Transform twrTrans;
	twrTrans.scale = glm::vec3(0.125f, 0.5f, 0.125f);
	twrTrans.position = glm::vec3(-0.6, 0, -0.75);
	m_tower1 = new Tower();
	m_tower1->bottomHeight = -0.45f;
	m_tower1->topHeight = 0.45f;
	m_tower1->releaseHeight = 0.75f;
	m_tower1->topRingState = TopRingState::BOTTOM;
	m_tower1->peg = new WorldObject(twrTrans, model_type::PEG);
	m_tower1->peg->setColor(glm::vec4(0.5450983f, 0.2705882f, 0.0745098f, m_selectedAlpha));
	m_worldObjects.push_back(m_tower1->peg);

	twrTrans.position = glm::vec3(0, 0, -0.75f);
	m_tower2 = new Tower();
	m_tower2->bottomHeight = -0.45f;
	m_tower2->topHeight = 0.45f;
	m_tower2->releaseHeight = 0.75f;
	m_tower2->topRingState = TopRingState::NONE;
	m_tower2->peg = new WorldObject(twrTrans, model_type::PEG);
	m_tower2->peg->setColor(glm::vec4(0.5450983f, 0.2705882f, 0.0745098f, m_defaultAlpha));
	m_worldObjects.push_back(m_tower2->peg);

	twrTrans.position = glm::vec3(0.6, 0, -0.75);
	m_tower3 = new Tower();
	m_tower3->bottomHeight = -0.45f;
	m_tower3->topHeight = 0.45f;
	m_tower3->releaseHeight = 0.75f;
	m_tower3->topRingState = TopRingState::NONE;
	m_tower3->peg = new WorldObject(twrTrans, model_type::PEG);
	m_tower3->peg->setColor(glm::vec4(0.5450983f, 0.2705882f, 0.0745098f, m_defaultAlpha));
	m_worldObjects.push_back(m_tower3->peg);

	// rings
	Transform ringTrans;
	ringTrans.position = glm::vec3(-0.6, m_tower1->bottomHeight, -1);
	ringTrans.scale = glm::vec3(0.25f, 0.25f, 0.25f);
	WorldObject* ring1 = new WorldObject(ringTrans, model_type::RING);
	ring1->setColor(glm::vec4((float)rand()/RAND_MAX + 0.1f, (float)rand()/RAND_MAX + 0.1f, (float)rand()/RAND_MAX+ 0.1f, m_defaultAlpha));
	m_worldObjects.push_back(ring1);
	m_tower1->rings.push(ring1);

	ringTrans.position = glm::vec3(-0.6, 0, -1);
	ringTrans.scale = glm::vec3(0.2f, 0.2f, 0.2f);
	WorldObject* ring2 = new WorldObject(ringTrans, model_type::RING);
	ring2->setColor(glm::vec4((float)rand()/RAND_MAX + 0.1f, (float)rand()/RAND_MAX + 0.1f, (float)rand()/RAND_MAX + 0.1f, m_defaultAlpha));
	m_worldObjects.push_back(ring2);
	m_tower1->rings.push(ring2);

	ringTrans.position = glm::vec3(-0.6, m_tower1->topHeight, -1);
	ringTrans.scale = glm::vec3(0.15f, 0.15f, 0.15f);
	WorldObject* ring3 = new WorldObject(ringTrans, model_type::RING);
	ring3->setColor(glm::vec4((float)rand()/RAND_MAX + 0.1f, (float)rand()/RAND_MAX + 0.1f, (float)rand()/RAND_MAX + 0.1f, m_selectedAlpha));
	m_worldObjects.push_back(ring3);
	m_tower1->rings.push(ring3);

	m_selectedTower = m_tower1;
}

Game::~Game()
{
	delete m_tower1;
	delete m_tower2;
	delete m_tower3;

	for each (auto obj in m_worldObjects)
	{
		delete obj;
	}

	m_worldObjects.clear();
}

void Game::update(float deltaTime, int keyPressed)
{
	if (keyPressed == GLFW_KEY_A && canMoveDelay == 0)
	{
		canMoveDelay = 0.25f;
		moveLeft();
	}
	else if (keyPressed == GLFW_KEY_S  && canMoveDelay == 0)
	{
		canMoveDelay = 0.25f;
		moveRingUpDown(::DOWN);
	}
	else if (keyPressed == GLFW_KEY_D  && canMoveDelay == 0)
	{
		canMoveDelay = 0.25f;
		moveRight();
	}
	else if (keyPressed == GLFW_KEY_W  && canMoveDelay == 0)
	{
		canMoveDelay = 0.25f;
		moveRingUpDown(::UP);
	}

	if (canMoveDelay > 0)
	{
		canMoveDelay-= deltaTime;
		if (canMoveDelay < 0)
		{
			canMoveDelay = 0;
		}
	}
}

void Game::moveRingUpDown(MoveDirection direction)
{
	if (m_selectedTower->rings.size() > 0)
	{
		auto topRing = m_selectedTower->rings.top();
		TopRingState ringState = getTopRingState(m_selectedTower);

		if (topRing)
		{
			bool canMove = false;

			switch (direction)
			{
			case MoveDirection::UP:		
				canMove = checkBounds(::UP);
				break;
			case MoveDirection::DOWN:
				canMove = checkBounds(::DOWN);
				break;
			default:
				break;
			}

			if (canMove)
			{
				if (direction == ::UP)
				{
					if (ringState == TopRingState::BOTTOM)
					{
						m_selectedTower->topRingState = ::MIDDLE;

						auto currentPosition = topRing->getTransform().position;
						currentPosition.y = 0;

						topRing->setPosition(currentPosition);
					}
					else if (ringState == TopRingState::MIDDLE)
					{
						m_selectedTower->topRingState = ::TOP;

						auto currentPosition = topRing->getTransform().position;
						currentPosition.y = m_selectedTower->topHeight;

						topRing->setPosition(currentPosition);
					}
					else if (ringState == TopRingState::TOP)
					{
						m_selectedTower->topRingState = ::RELEASED;

						auto currentPosition = topRing->getTransform().position;
						currentPosition.y = m_selectedTower->releaseHeight;

						topRing->setPosition(currentPosition);
					}
				}
				else if (direction == ::UP)
				{
					if (ringState == TopRingState::RELEASED)
					{
						m_selectedTower->topRingState = ::TOP;

						auto currentPosition = topRing->getTransform().position;
						currentPosition.y = m_selectedTower->topHeight;

						topRing->setPosition(currentPosition);
					}
					else if (ringState == TopRingState::TOP)
					{
						m_selectedTower->topRingState = ::MIDDLE;

						auto currentPosition = topRing->getTransform().position;
						currentPosition.y = 0;

						topRing->setPosition(currentPosition);
					}
					else if (ringState == TopRingState::MIDDLE)
					{
						m_selectedTower->topRingState = ::BOTTOM;

						auto currentPosition = topRing->getTransform().position;
						currentPosition.y = m_selectedTower->bottomHeight;

						topRing->setPosition(currentPosition);
					}
				}
			}
		}
	}
}

void Game::selectTower(Tower* tower, bool moveRing)
{
	clearSelections();

	if (m_selectedTower)
	{
		auto currentColor = m_selectedTower->peg->getColor();
		currentColor.a = m_defaultAlpha;
		m_selectedTower->peg->setColor(currentColor);

		if (moveRing)
		{
			auto topRing = m_selectedTower->rings.top();

			if (topRing)
			{						
				int ringCountInNewTower = tower->rings.size();

				m_selectedTower->rings.pop();

				glm::vec3 newPos = tower->peg->getTransform().position;
				newPos.y = tower->releaseHeight;

				topRing->setPosition(newPos);
			}
		}
	}

	auto currentColor = tower->peg->getColor();
	currentColor.a = m_selectedAlpha;
	tower->peg->setColor(currentColor);

	m_selectedTower = tower;
}

void Game::clearSelections()
{
	// tower 1
	auto currentColor = m_tower1->peg->getColor();
	currentColor.a = m_defaultAlpha;
	m_tower1->peg->setColor(currentColor);

	if (m_tower1->rings.size() > 0)
	{
		auto topRing = m_tower1->rings.top();
		currentColor = topRing->getColor();
		currentColor.a = m_defaultAlpha;
		topRing->setColor(currentColor);
	}	

	// tower 2
	currentColor = m_tower2->peg->getColor();
	currentColor.a = m_defaultAlpha;
	m_tower2->peg->setColor(currentColor);

	if (m_tower2->rings.size() > 0)
	{
		auto topRing = m_tower2->rings.top();
		currentColor = topRing->getColor();
		currentColor.a = m_defaultAlpha;
		topRing->setColor(currentColor);
	}

	// tower 1
	currentColor = m_tower3->peg->getColor();
	currentColor.a = m_defaultAlpha;
	m_tower3->peg->setColor(currentColor);

	if (m_tower3->rings.size() > 0)
	{
		auto topRing = m_tower3->rings.top();
		currentColor = topRing->getColor();
		currentColor.a = m_defaultAlpha;
		topRing->setColor(currentColor);
	}
}

void Game::moveLeft()
{
	bool moveRing = checkBounds(::LEFT);

	if (m_selectedTower == m_tower3)
	{
		selectTower(m_tower2, moveRing);
	}
	else if (m_selectedTower == m_tower2)
	{
		selectTower(m_tower1, moveRing);
	}
}

void Game::moveRight()
{
	bool moveRing = checkBounds(::LEFT);

	if (m_selectedTower == m_tower1)
	{
		selectTower(m_tower2, moveRing);
	}
	else if (m_selectedTower == m_tower2)
	{
		selectTower(m_tower3, moveRing);
	}
}

TopRingState Game::getTopRingState(Tower* tower)
{
	return tower->topRingState;
}

bool Game::checkBounds(MoveDirection direction)
{
	if (m_selectedTower->rings.size() > 0)
	{	
		auto topRing = m_selectedTower->rings.top();
		auto topRingState = getTopRingState(m_selectedTower);

		if (topRing)
		{
			switch (direction)
			{
			case ::LEFT:
				if (m_selectedTower == m_tower2 || m_selectedTower == m_tower3
					&& topRingState == ::RELEASED)
				{
					return true;
				}
				break;
			case ::RIGHT:
				if (m_selectedTower == m_tower1 || m_selectedTower == m_tower2
					&& topRingState == ::RELEASED)
				{
					return true;
				}
				break;
			case ::UP:
				if (!(topRingState == ::RELEASED) && !(topRingState == ::NONE))
				{
					return true;
				}
				break;
			case ::DOWN:
				if (!(topRingState == ::BOTTOM) && !(topRingState == ::NONE))
				{
					return true;
				}
				break;
			}
		}	
	}

	return false;
}
