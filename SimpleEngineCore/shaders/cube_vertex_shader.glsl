#version 460

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 texture_coord;

uniform mat4 model_view_mat; // go into camera space
uniform mat4 mvp_mat; // model * view * proj mat
uniform mat3 normal_matrix;	// mat for normal; it has to be slightly different from our mv matrix because of transforamtion of space and normals should be transformed a bit differently from 38 lecture SimpleCoding 	 
uniform int current_frame;

out vec2 tex_coord_smile;
out vec3 frag_normal_eye; // in camera space
out vec3 frag_pos_eye; // in camera space

void main() {
	tex_coord_smile = texture_coord;

	frag_normal_eye = normal_matrix * vertex_normal;
	frag_pos_eye = vec3(model_view_mat * vec4(vertex_position, 1.0));
	gl_Position = mvp_mat * vec4(vertex_position, 1.0);
}