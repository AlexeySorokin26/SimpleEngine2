#version 460

layout(location = 0) in vec3 vertex_position;

uniform mat4 mvp_mat;

void main() 
{
	gl_Position = mvp_mat * vec4(
		vertex_position.x * 50, vertex_position.y * 50, vertex_position.z * 1, 1.0f);
}