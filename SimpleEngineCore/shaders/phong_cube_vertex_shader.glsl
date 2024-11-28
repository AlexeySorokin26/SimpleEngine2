#version 460

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 texture_coord;

uniform mat4 m_mat;
uniform mat3 normal_mat; 
uniform mat4 mvp_mat;

out vec3 frag_pos;
out vec3 frag_normal;
out vec2 tex_coord;

void main() {
	tex_coord = texture_coord;

	frag_pos = vec3(m_mat * vec4(vertex_position, 1.0)); // we don't need project here 
	frag_normal = normal_mat * vertex_normal;
	gl_Position = mvp_mat * vec4(vertex_position, 1.0); // projected too
}