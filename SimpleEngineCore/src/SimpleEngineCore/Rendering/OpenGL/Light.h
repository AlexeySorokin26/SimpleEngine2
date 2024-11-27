#pragma once

#include <glm/vec3.hpp>
#include <glad/glad.h>

namespace SimpleEngine {

	class Light {
	public:
		Light(
			const glm::vec3& ambient = glm::vec3(1.0f, 1.0f, 1.0f),
			const glm::vec3& diffuse = glm::vec3(1.0f, 1.0f, 1.0f),
			const glm::vec3& specular = glm::vec3(1.0f, 1.0f, 1.0f),
			float ambientIntensity = 0.1f,
			float diffuseIntensity = 0.4f,
			float specularIntensity = 0.6f
		)
			: ambient(ambient), diffuse(diffuse), specular(specular),
			ambientIntensity(ambientIntensity), diffuseIntensity(diffuseIntensity), specularIntensity(specularIntensity)
		{
		}

		virtual ~Light() = default;

		virtual void UseLight(
			GLint ambientLoc, GLint diffuseLoc, GLint specularLoc,
			GLint ambientIntensityLoc, GLint diffuseIntensityLoc, GLint specularIntensityLoc
		) const {
			glUniform3f(ambientLoc, ambient.x, ambient.y, ambient.z);
			glUniform3f(diffuseLoc, diffuse.x, diffuse.y, diffuse.z);
			glUniform3f(specularLoc, specular.x, specular.y, specular.z);

			glUniform1f(ambientIntensityLoc, ambientIntensity);
			glUniform1f(diffuseIntensityLoc, diffuseIntensity);
			glUniform1f(specularIntensityLoc, specularIntensity);
		}

	protected:
		glm::vec3 ambient;
		float ambientIntensity;
		glm::vec3 diffuse;
		float diffuseIntensity;
		glm::vec3 specular;
		float specularIntensity;
	};

	class DirectionalLight : public Light {
	public:
		DirectionalLight(
			const glm::vec3& direction = glm::vec3(0.0f, 1.0f, 0.0f),
			const glm::vec3& ambient = glm::vec3(1.0f, 1.0f, 1.0f),
			const glm::vec3& diffuse = glm::vec3(1.0f, 1.0f, 1.0f),
			const glm::vec3& specular = glm::vec3(1.0f, 1.0f, 1.0f),
			float ambientIntensity = 0.1f,
			float diffuseIntensity = 0.4f,
			float specularIntensity = 0.6f
		)
			: Light(ambient, diffuse, specular, ambientIntensity, diffuseIntensity, specularIntensity),
			direction(direction)
		{
		}

		void UseLight(
			GLint ambientLoc, GLint diffuseLoc, GLint specularLoc,
			GLint directionLoc, GLint ambientIntensityLoc, GLint diffuseIntensityLoc, GLint specularIntensityLoc
		) const {
			Light::UseLight(ambientLoc, diffuseLoc, specularLoc, ambientIntensityLoc, diffuseIntensityLoc, specularIntensityLoc);
			glUniform3f(directionLoc, direction.x, direction.y, direction.z);
		}

		glm::vec3 direction;
	};

	class PointLight : public Light {
	public:
		PointLight(
			const glm::vec3& position = glm::vec3(0.0f, 1.0f, 0.0f),
			const glm::vec3& ambient = glm::vec3(1.0f, 1.0f, 1.0f),
			const glm::vec3& diffuse = glm::vec3(1.0f, 1.0f, 1.0f),
			const glm::vec3& specular = glm::vec3(1.0f, 1.0f, 1.0f),
			float ambientIntensity = 0.1f,
			float diffuseIntensity = 0.4f,
			float specularIntensity = 0.6f,
			float constant = 1.0f,
			float linear = 0.09f,
			float quadratic = 0.032f
		)
			: Light(ambient, diffuse, specular, ambientIntensity, diffuseIntensity, specularIntensity),
			position(position), constant(constant), linear(linear), quadratic(quadratic)
		{
		}

		void UseLight(
			GLint ambientLoc, GLint diffuseLoc, GLint specularLoc,
			GLint positionLoc, GLint ambientIntensityLoc, GLint diffuseIntensityLoc, GLint specularIntensityLoc,
			GLint constantLoc, GLint linearLoc, GLint quadraticLoc
		) const {
			Light::UseLight(ambientLoc, diffuseLoc, specularLoc, ambientIntensityLoc, diffuseIntensityLoc, specularIntensityLoc);
			glUniform3f(positionLoc, position.x, position.y, position.z);
			glUniform1f(constantLoc, constant);
			glUniform1f(linearLoc, linear);
			glUniform1f(quadraticLoc, quadratic);
		}

		glm::vec3 position;
		float constant;
		float linear;
		float quadratic;
	};

}