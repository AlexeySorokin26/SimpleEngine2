#version 460

uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;
uniform vec3 light_pos;

out vec4 frag_color;

void main() {
	frag_color = vec4(0.5 * light_ambient + 0.5 * light_diffuse, 1.0);
}