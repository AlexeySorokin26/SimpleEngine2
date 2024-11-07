#version 460

in vec2 tex_coord_smile;
in vec3 frag_pos_eye; // now we got data in cam space 
in vec3 frag_normal_eye;

layout(binding = 0) uniform sampler2D InTexture_Smile;

uniform vec3 light_color;
uniform vec3 light_pos_eye;

uniform float ambient_factor;
uniform float diffuse_factor;
uniform float specular_factor;
uniform float shininess;

out vec4 frag_color;

void main() {
	// ambient
	vec3 ambient = ambient_factor * light_color;

	// diffuse
	vec3 normal = normalize(frag_normal_eye);
	vec3 light_direction = normalize(light_pos_eye - frag_pos_eye);
	vec3 diffuse = diffuse_factor * light_color * max(dot(normal, light_direction), 0);

	// specular 
	vec3 view_dir = normalize(-frag_pos_eye); // vector to cam
	vec3 reflect_dir = reflect(-light_direction, normal);
	vec3 specular = specular_factor * light_color * pow(max(dot(view_dir, reflect_dir), 0), shininess);

	frag_color = vec4((ambient + diffuse + specular), 1.f) * texture(InTexture_Smile, tex_coord_smile);
}