#pragma once

#include <unordered_map>

enum shader_type
{
	GENERAL_DIFFUSE
};

enum shader_uniform
{
	COLOR, MVP
};

struct Shader
{
	shader_type type;
	unsigned int program;

	std::unordered_map<shader_uniform, unsigned int*> uniforms;
};