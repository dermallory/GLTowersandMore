#pragma once

#include <shader.h>

enum model_type
{
	RING, PEG
};

struct Model
{
	model_type type;
	shader_type shader;
	unsigned int vao;
};