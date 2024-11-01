#version 460

uniform vec3 ground_color;

out vec4 frag_color;

void main()
{
	frag_color = vec4(ground_color, 1.0);
}