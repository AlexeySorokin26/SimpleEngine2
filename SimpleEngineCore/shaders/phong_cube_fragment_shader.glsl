#version 460

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 tex_coord;

layout(binding = 0) uniform sampler2D InTexture;
layout(binding = 1) uniform sampler2D InTexture1;

uniform vec3 cube_color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 cam_pos;
uniform float ambient_factor;
uniform float diffuse_factor;
uniform float specular_factor;
uniform float shininess;

out vec4 frag_color;

void main() {
	// ambient
	vec3 ambient_light = ambient_factor * light_color;

	// diffuse 
	vec3 normal = normalize(frag_normal);
	vec3 light_direction = normalize(light_pos - frag_pos);
	vec3 diffuse_light = diffuse_factor * light_color * max(dot(light_direction, normal), 0);

	// specular
	vec3 view_direction = normalize(cam_pos - frag_pos);
	vec3 reflected_direction = reflect(-light_direction, normal);
	vec3 specular_light = specular_factor * light_color * pow(max(dot(reflected_direction, view_direction), 0), shininess);

	vec4 color = texture(InTexture, tex_coord);
	vec4 color1 = texture(InTexture1, tex_coord);
	frag_color = vec4(ambient_light + diffuse_light + specular_light, 1.0) * /*vec4(cube_color, 1.0) **/  mix(color, color1, 0.5);;
}