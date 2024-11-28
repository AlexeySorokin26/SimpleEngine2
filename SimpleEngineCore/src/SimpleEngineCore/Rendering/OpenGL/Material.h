#pragma once

#include <glm/vec3.hpp>

namespace SimpleEngine {

	class Material {
	public:
		Material(
			const glm::vec3& ambient = glm::vec3(1.0f, 1.0f, 1.0f),
			const float shininess = 1.f
		)
			: ambient(ambient), shininess(shininess)
		{
		}
		~Material() {}
		glm::vec3 ambient{ 0.1f, 0.1f, 0.1f };
		float shininess = { 32.f };

		void UseMaterial(
			GLint ambient_loc = 0,
			GLint shininess_loc = 0) const
		{
			glUniform3f(ambient_loc, ambient.x, ambient.y, ambient.z);
			glUniform1f(shininess_loc, shininess);
		}
	};
}
