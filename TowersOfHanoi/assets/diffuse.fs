#version 450 core

in Frag_In
{
	vec4 color;
	vec3 normal;
} frag_out;

out vec3 color;

void main(void)
{
	color = frag_out.color.rgb * frag_out.color.a;
}