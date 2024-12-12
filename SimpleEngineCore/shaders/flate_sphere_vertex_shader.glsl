#version 460

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;

out vec3 frag_pos;
flat out vec3 frag_normal;

uniform mat4 m_mat;
uniform mat3 normal_mat; 
uniform mat4 mvp_mat;

void main() {
	frag_pos = vec3(m_mat * vec4(vertex_position, 1.0)); // Transform to world space
	frag_normal = normal_mat * vertex_normal;			 // Transform normal
	gl_Position = mvp_mat * vec4(vertex_position, 1.0);  //  Transform to clip space
}