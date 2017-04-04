#pragma once

#include <vector>

typedef unsigned int keyboard_key;

struct InputHandler
{
	std::vector<keyboard_key> pressed;
	std::vector<keyboard_key> down;
	std::vector<keyboard_key> released;
};
