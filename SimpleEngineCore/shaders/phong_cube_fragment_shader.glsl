#version 460

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 tex_coord;

layout(binding = 0) uniform sampler2D InTexture;
layout(binding = 1) uniform sampler2D InTexture1;


struct DirectionalLight
{
	vec3 ambient;
	float ambientIntensity;
	vec3 diffuse;
	float diffuseIntensity;
	vec3 specular;
	float specularIntensity;
	vec3 direction;
};

struct PointLight
{
	vec3 ambient;
	float ambientIntensity;
	vec3 diffuse;
	float diffuseIntensity;
	vec3 specular;
	float specularIntensity;
	vec3 position;

	float constant;
	float linear;
	float quadratic;
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
uniform PointLight pointLight;
uniform Material material;
uniform vec3 cam_pos;

out vec4 frag_color;


// Function to compute point light contribution
vec3 ComputePointLight(vec3 frag_pos, vec3 frag_normal, vec3 view_direction) {
    // Distance and attenuation
    float distance = length(pointLight.position - frag_pos);
    float attenuation = 1.0 / (pointLight.constant +
        pointLight.linear * distance +
        pointLight.quadratic * (distance * distance));

    // Ambient
    vec3 ambient = attenuation * pointLight.ambientIntensity * pointLight.ambient * material.ambient;

    // Diffuse
    vec3 light_direction = normalize(pointLight.position - frag_pos);
    vec3 diffuse = attenuation * pointLight.diffuseIntensity * pointLight.diffuse * material.diffuse *
        max(dot(light_direction, frag_normal), 0.0);

    // Specular
    vec3 reflected_direction = reflect(-light_direction, frag_normal);
    vec3 specular = attenuation * pointLight.specularIntensity * pointLight.specular * material.specular *
        pow(max(dot(reflected_direction, view_direction), 0.0), material.shininess);

    return ambient + diffuse + specular;
}

// Function to compute directional light contribution
vec3 ComputeDirectionalLight(vec3 frag_normal, vec3 view_direction) {
    // Ambient
    vec3 ambient = directionalLight.ambientIntensity * directionalLight.ambient * material.ambient;

    // Diffuse
    vec3 light_direction = normalize(-directionalLight.direction);
    vec3 diffuse = directionalLight.diffuseIntensity * directionalLight.diffuse * material.diffuse *
        max(dot(light_direction, frag_normal), 0.0);

    // Specular
    vec3 reflected_direction = reflect(-light_direction, frag_normal);
    vec3 specular = directionalLight.specularIntensity * directionalLight.specular * material.specular *
        pow(max(dot(reflected_direction, view_direction), 0.0), material.shininess);

    return ambient + diffuse + specular;
}

void main() {
    // Normalize inputs
    vec3 normal = normalize(frag_normal);
    vec3 view_direction = normalize(cam_pos - frag_pos);

    // Compute lighting
    vec3 point_light = ComputePointLight(frag_pos, normal, view_direction);
    vec3 directional_light = ComputeDirectionalLight(normal, view_direction);

    // Combine lights
    vec3 total_light = globalAmbient * material.ambient + point_light /*+ directional_light*/;

    // Texture mapping and output
    vec4 colorTex = texture(InTexture, tex_coord);
    vec4 colorTex1 = texture(InTexture1, tex_coord);
    frag_color = vec4(total_light, 1.0) * mix(colorTex, colorTex1, 0.5);
}
