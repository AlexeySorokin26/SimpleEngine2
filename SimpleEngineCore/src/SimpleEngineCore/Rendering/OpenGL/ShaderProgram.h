#pragma once

#include <glm/mat4x4.hpp>

namespace SimpleEngine {

	class ShaderProgram {
	public:
		ShaderProgram(const char* vertex_shader_src, const char* frag_shader_src);
		ShaderProgram(ShaderProgram&&);
		ShaderProgram& operator=(ShaderProgram&&);
		~ShaderProgram();

		ShaderProgram() = delete; // we need some text to start work without no sense 
		ShaderProgram(const ShaderProgram& shaderProgram) = delete;
		ShaderProgram& operator=(const ShaderProgram& shaderProgram) = delete;

		void bind() const;
		static void unbind();
		bool is_compiled() const { return m_isCompiled; }

		void set_matrix4(const char* name, const glm::mat4& matrix) const;
		void set_matrix3(const char* name, const glm::mat3& matrix) const;
		void set_int(const char* name, const int value) const;
		void set_float(const char* name, const float value) const;
		void set_vec3(const char* name, const glm::vec3& v) const;

	private:
		bool m_isCompiled = false;
		unsigned int m_id = 0;
	};
}