#pragma once

#include <glm/vec3.hpp>
#include <glad/glad.h>

namespace SimpleEngine {
	class DirectionalLight {
	public:
		DirectionalLight(
			const glm::vec3& direction = glm::vec3(.0f, 1.0f, .0f),
			const glm::vec3& ambient = glm::vec3(1.0f, 1.0f, 1.0f),
			const glm::vec3& diffuse = glm::vec3(1.0f, 1.0f, 1.0f),
			const glm::vec3& specular = glm::vec3(1.0f, 1.0f, 1.0f),
			float ambientIntensity = 0.1,
			float diffuseIntensity = 0.4,
			float specularIntensity = 0.6
		)
			: direction(direction), ambient(ambient), diffuse(diffuse), specular(specular),
			ambientIntensity(ambientIntensity), diffuseIntensity(diffuseIntensity), specularIntensity(specularIntensity)
		{
		}
		~DirectionalLight() {}

		void UseLight(
			GLint ambientLoc = 0,
			GLint diffuseLoc = 0,
			GLint specularLoc = 0,
			GLint directionLoc = 0,
			GLint ambientIntensityLoc = 0,
			GLint diffuseIntensityLoc = 0,
			GLint specularIntensityLoc = 0
		) const
		{
			glUniform3f(ambientLoc, ambient.x, ambient.y, ambient.z);
			glUniform3f(diffuseLoc, diffuse.x, diffuse.y, diffuse.z);
			glUniform3f(specularLoc, specular.x, specular.y, specular.z);
			glUniform3f(directionLoc, direction.x, direction.y, direction.z);

			glUniform1f(ambientIntensityLoc, ambientIntensity);
			glUniform1f(diffuseIntensityLoc, diffuseIntensity);
			glUniform1f(specularIntensityLoc, specularIntensity);
		}

		glm::vec3 direction;
		glm::vec3 ambient;
		float ambientIntensity;
		glm::vec3 diffuse;
		float diffuseIntensity;
		glm::vec3 specular;
		float specularIntensity;
	};

	class PointLight {
	public:
		PointLight(
			const glm::vec3& position = glm::vec3(.0f, 1.0f, .0f),
			const glm::vec3& ambient = glm::vec3(1.0f, 1.0f, 1.0f),
			const glm::vec3& diffuse = glm::vec3(1.0f, 1.0f, 1.0f),
			const glm::vec3& specular = glm::vec3(1.0f, 1.0f, 1.0f),
			float ambientIntensity = 0.1,
			float diffuseIntensity = 0.4,
			float specularIntensity = 0.6,
			float constant = 1.0f,
			float linear = 0.09f,
			float quadratic = 0.032f
		)
			: position(position), ambient(ambient), diffuse(diffuse), specular(specular),
			ambientIntensity(ambientIntensity), diffuseIntensity(diffuseIntensity), specularIntensity(specularIntensity),
			constant(constant), linear(linear), quadratic(quadratic)
		{
		}
		~PointLight() {}

		void UseLight(
			GLint ambientLoc = 0,
			GLint diffuseLoc = 0,
			GLint specularLoc = 0,
			GLint positionLoc = 0,
			GLint ambientIntensityLoc = 0,
			GLint diffuseIntensityLoc = 0,
			GLint specularIntensityLoc = 0,
			GLint constantLoc = 0,
			GLint linearLoc = 0,
			GLint quadraticLoc = 0
		) const
		{
			glUniform3f(ambientLoc, ambient.x, ambient.y, ambient.z);
			glUniform3f(diffuseLoc, diffuse.x, diffuse.y, diffuse.z);
			glUniform3f(specularLoc, specular.x, specular.y, specular.z);
			glUniform3f(positionLoc, position.x, position.y, position.z);

			glUniform1f(ambientIntensityLoc, ambientIntensity);
			glUniform1f(diffuseIntensityLoc, diffuseIntensity);
			glUniform1f(specularIntensityLoc, specularIntensity);

			glUniform1f(constantLoc, constant);
			glUniform1f(linearLoc, linear);
			glUniform1f(quadraticLoc, quadratic);
		}

		glm::vec3 position;
		glm::vec3 ambient;
		float ambientIntensity;
		glm::vec3 diffuse;
		float diffuseIntensity;
		glm::vec3 specular;
		float specularIntensity;

		float constant;
		float linear;
		float quadratic;
	};
}