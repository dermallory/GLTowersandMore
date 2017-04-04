#version 450 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

uniform vec4 color;
uniform mat4 mvp;

// get from a Uniform Buffer Object
//layout(std140, binding = 0) uniform Lighting 
//{
//	vec4 directional;
//	vec4 ambient;
//} model;

out Frag_In {
	vec4 color;
	vec3 normal;
} frag_out;

void main(void)
{
	gl_Position = mvp * position;

	frag_out.normal = normal;
	frag_out.color = color;
}
