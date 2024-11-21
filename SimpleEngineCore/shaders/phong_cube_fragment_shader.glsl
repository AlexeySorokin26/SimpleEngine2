#version 460

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 tex_coord;

layout(binding = 0) uniform sampler2D InTexture;
layout(binding = 1) uniform sampler2D InTexture1;


struct DirectionalLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 pos;
};

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform vec3 globalAmbient;
uniform DirectionalLight directionalLight;
uniform Material material;
uniform vec3 cam_pos;

out vec4 frag_color;

void main() {
	// ambient
	vec3 ambient_light = globalAmbient * material.ambient + directionalLight.ambient * material.ambient;

	// diffuse 
	vec3 normal = normalize(frag_normal);
	vec3 light_direction = normalize(directionalLight.pos - frag_pos);
	vec3 diffuse_light = directionalLight.diffuse * material.diffuse * max(dot(light_direction, normal), 0);

	// specular
	vec3 view_direction = normalize(cam_pos - frag_pos);
	vec3 reflected_direction = reflect(-light_direction, normal);
	vec3 specular_light = 
		directionalLight.specular * material.specular * pow(max(dot(reflected_direction, view_direction), 0), material.shininess);

	vec4 colorTex = texture(InTexture, tex_coord);
	vec4 colorTex1 = texture(InTexture1, tex_coord);
	frag_color = vec4(ambient_light + diffuse_light + specular_light, 1.0) * mix(colorTex, colorTex1, 0.5);;
}