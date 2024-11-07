#version 460

uniform vec3 ground_color;
uniform float ambient_factor;
uniform vec3 light_color;

out vec4 frag_color;

void main()
{
	// ambient
	vec3 ambient = ambient_factor * light_color;

	//// diffuse
	//vec3 normal = normalize(frag_normal_eye);
	//vec3 light_direction = normalize(light_pos_eye - frag_pos_eye);
	//vec3 diffuse = diffuse_factor * light_color * max(dot(normal, light_direction), 0);

	//// specular 
	//vec3 view_dir = normalize(-frag_pos_eye); // vector to cam
	//vec3 reflect_dir = reflect(-light_direction, normal);
	//vec3 specular = specular_factor * light_color * pow(max(dot(view_dir, reflect_dir), 0), shininess);

	//frag_color = vec4((ambient + diffuse + specular), 1.f) * texture(InTexture_Smile, tex_coord_smile);
	frag_color = vec4(ambient, 1.f) * vec4(ground_color, 1.f);
}