#version 460

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 tex_coord;

struct DirectionalLight {
    vec3 ambient;
    float ambientIntensity;
    vec3 diffuse;
    float diffuseIntensity;
    vec3 specular;
    float specularIntensity;

    vec3 direction;
};

struct PointLight {
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

struct SpotLight {
    vec3 ambient;
    float ambientIntensity;
    vec3 diffuse;
    float diffuseIntensity;
    vec3 specular;
    float specularIntensity;

    vec3 position;
    vec3 direction;

    float constant;
    float linear;
    float quadratic;

    float cutOff;  // result of cos(cutoff)
};

struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform vec3 globalAmbient;
uniform DirectionalLight directionalLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 cam_pos;
uniform bool useDirLight;
uniform bool usePointLight;
uniform bool useSpotLight;

out vec4 frag_color;

// Function to compute point light contribution
vec3 ComputePointLight(vec3 frag_pos, vec3 frag_normal, vec3 view_direction, vec3 sampledDiffuse, vec3 sampledSpecular) {
    // Distance and attenuation
    float distance = length(pointLight.position - frag_pos);
    float attenuation = 1.0 / (pointLight.constant +
        pointLight.linear * distance +
        pointLight.quadratic * (distance * distance));

    // Diffuse
    vec3 light_direction = normalize(pointLight.position - frag_pos);
    vec3 diffuse = attenuation * pointLight.diffuseIntensity * pointLight.diffuse *
        sampledDiffuse * max(dot(light_direction, frag_normal), 0.0);

    // Specular
    vec3 reflected_direction = reflect(-light_direction, frag_normal);
    vec3 specular = attenuation * pointLight.specularIntensity * pointLight.specular *
        sampledSpecular * pow(max(dot(reflected_direction, view_direction), 0.0), material.shininess);

    return diffuse + specular;
}

// Function to compute directional light contribution
vec3 ComputeDirectionalLight(vec3 frag_normal, vec3 view_direction, vec3 sampledDiffuse, vec3 sampledSpecular) {
    // Diffuse
    vec3 light_direction = normalize(-directionalLight.direction);
    vec3 diffuse = directionalLight.diffuseIntensity * directionalLight.diffuse *
        sampledDiffuse * max(dot(light_direction, frag_normal), 0.0);

    // Specular
    vec3 reflected_direction = reflect(-light_direction, frag_normal);
    vec3 specular = directionalLight.specularIntensity * directionalLight.specular *
        sampledSpecular * pow(max(dot(reflected_direction, view_direction), 0.0), material.shininess);

    return diffuse + specular;
}

// Function to compute spot light contribution
vec3 ComputeSpotLight(vec3 frag_pos, vec3 frag_normal, vec3 view_direction, vec3 sampledDiffuse, vec3 sampledSpecular) {
    vec3 frag_direction = normalize(spotLight.position - frag_pos);
    float theta = dot(frag_direction, normalize(-spotLight.direction));

    // Distance and attenuation
    float distance = length(spotLight.position - frag_pos);
    float attenuation = 1.0 / (spotLight.constant +
        spotLight.linear * distance +
        spotLight.quadratic * (distance * distance));

    if (theta > spotLight.cutOff) {
        // Diffuse
        vec3 light_direction = normalize(spotLight.position - frag_pos);
        vec3 diffuse = attenuation * spotLight.diffuseIntensity * spotLight.diffuse *
            sampledDiffuse * max(dot(light_direction, frag_normal), 0.0);

        // Specular
        vec3 reflected_direction = reflect(-light_direction, frag_normal);
        vec3 specular = attenuation * spotLight.specularIntensity * spotLight.specular *
            sampledSpecular * pow(max(dot(reflected_direction, view_direction), 0.0), material.shininess);

        return diffuse + specular;
    }
    return vec3(0.0);  // No contribution outside the spotlight's cutoff
}

void main() {
    // Normalize inputs
    vec3 normal = normalize(frag_normal);
    vec3 view_direction = normalize(cam_pos - frag_pos);

    // Sample textures once
    vec3 sampledDiffuse = vec3(texture(material.diffuse, tex_coord));
    vec3 sampledSpecular = vec3(texture(material.specular, tex_coord));

    // Compute global ambient lighting
    vec3 ambient_light = globalAmbient * sampledDiffuse;

    if (useDirLight) {
        ambient_light += directionalLight.ambientIntensity * directionalLight.ambient * sampledDiffuse;
    }
    if (usePointLight) {
        ambient_light += pointLight.ambientIntensity * pointLight.ambient * sampledDiffuse;
    }
    if (useSpotLight) {
        ambient_light += spotLight.ambientIntensity * spotLight.ambient * sampledDiffuse;
    }

    // Compute light contributions
    vec3 total_light = ambient_light;

    if (useDirLight) {
        total_light += ComputeDirectionalLight(normal, view_direction, sampledDiffuse, sampledSpecular);
    }
    if (usePointLight) {
        total_light += ComputePointLight(frag_pos, normal, view_direction, sampledDiffuse, sampledSpecular);
    }
    if (useSpotLight) {
        total_light += ComputeSpotLight(frag_pos, normal, view_direction, sampledDiffuse, sampledSpecular);
    }

    // Set final fragment color
    frag_color = vec4(total_light, 1.0);
}
