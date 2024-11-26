#pragma once

#include <glm/vec3.hpp>

namespace SimpleEngine {
	// TODO use lightmaps 
	// TODO add simple material too
	class Material {
	public:
		Material(
			const glm::vec3& ambient = glm::vec3(1.0f, 1.0f, 1.0f),
			const glm::vec3& diffuse = glm::vec3(1.0f, 1.0f, 1.0f),
			const glm::vec3& specular = glm::vec3(1.0f, 1.0f, 1.0f),
			const float shininess = 1.f
		)
			: ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
		{
		}
		~Material() {}
		glm::vec3 ambient{ 0.1f, 0.1f, 0.1f };
		glm::vec3 diffuse{ 1.1f, 1.1f, 1.1f };
		glm::vec3 specular{ 0.5f, 0.5f, 0.5f };
		float shininess = { 32.f };

		void UseMaterial(
			GLint ambient_loc = 0,
			GLint diffuse_loc = 0,
			GLint specular_loc = 0,
			GLint shininess_loc = 0) const
		{
			glUniform3f(ambient_loc, ambient.x, ambient.y, ambient.z);
			glUniform3f(diffuse_loc, diffuse.x, diffuse.y, diffuse.z);
			glUniform3f(specular_loc, specular.x, specular.y, specular.z);
			glUniform1f(shininess_loc, shininess);
		}
	};
}
