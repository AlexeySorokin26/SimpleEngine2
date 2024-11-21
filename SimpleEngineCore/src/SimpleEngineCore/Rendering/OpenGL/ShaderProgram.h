#pragma once

#include <glm/mat4x4.hpp>

#include <string>

#include <glad/glad.h>

namespace SimpleEngine {

	class ShaderProgram {
	public:
		ShaderProgram(const std::string& file_vertex_shader, const std::string& file_frag_shader);
		ShaderProgram(ShaderProgram&&);
		ShaderProgram& operator=(ShaderProgram&&);
		~ShaderProgram();

		ShaderProgram() = delete; // we need some text to start work without no sense 
		ShaderProgram(const ShaderProgram& shaderProgram) = delete;
		ShaderProgram& operator=(const ShaderProgram& shaderProgram) = delete;

		std::string ReadFile(const std::string& fileLocation);

		void bind() const;
		static void unbind();
		bool is_compiled() const { return m_isCompiled; }

		void set_matrix4(const char* name, const glm::mat4& matrix) const;
		void set_matrix3(const char* name, const glm::mat3& matrix) const;
		void set_int(const char* name, const int value) const;
		void set_float(const char* name, const float value) const;
		void set_vec3(const char* name, const glm::vec3& v) const;

		GLint get_uniform_location(const char* name) const;

	private:
		bool m_isCompiled = false;
		unsigned int m_id = 0;
	};
}