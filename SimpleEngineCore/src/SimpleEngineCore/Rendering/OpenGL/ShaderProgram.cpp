#include "ShaderProgram.h"

#include "SimpleEngineCore/Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>

namespace SimpleEngine {
	bool create_shader(const char* source, const GLenum shader_type, GLuint& shader_id) {
		// create shader, put source and compile it 
		shader_id = glCreateShader(shader_type);
		glShaderSource(shader_id, 1, &source, nullptr);
		glCompileShader(shader_id);

		GLint success;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success); // take GL_COMPILES_STATUS into success
		if (success == GL_FALSE) {
			char info_log[1024];
			glGetShaderInfoLog(shader_id, 1024, nullptr, info_log);
			LOG_CRIT("Shader compilation error:\n{}", info_log);
			return false;
		}
		return true;
	}

	ShaderProgram::ShaderProgram(const std::string& file_vertex_shader, const std::string& file_frag_shader)
	{
		GLuint vertex_shader_id = 0;
		if (!create_shader(ReadFile(file_vertex_shader).c_str(), GL_VERTEX_SHADER, vertex_shader_id)) {
			LOG_CRIT("Vertex shader: compile-time error!");
			glDeleteShader(vertex_shader_id);
			return;
		}

		GLuint frag_shader_id = 0;
		if (!create_shader(ReadFile(file_frag_shader).c_str(), GL_FRAGMENT_SHADER, frag_shader_id)) {
			LOG_CRIT("Frag shader: compile-time error!");
			glDeleteShader(vertex_shader_id);
			glDeleteShader(frag_shader_id);
			return;
		}


		m_id = glCreateProgram();
		glAttachShader(m_id, vertex_shader_id); // link vs
		glAttachShader(m_id, frag_shader_id); // link fs
		glLinkProgram(m_id); // create final programm

		GLint success;
		glGetProgramiv(m_id, GL_LINK_STATUS, &success);
		if (success == GL_FALSE) {
			GLchar info_log[1024];
			glGetProgramInfoLog(m_id, 1024, nullptr, info_log);
			LOG_CRIT("SHADER PROGRAM: Link-time error:\n{0}", info_log);
			glDeleteProgram(m_id);
			m_id = 0;
			glDeleteShader(vertex_shader_id);
			glDeleteShader(frag_shader_id);
			return;
		}
		else {
			m_isCompiled = true;
		}

		// need to detach shaders from programm fist because of refernce counting 
		glDetachShader(m_id, vertex_shader_id);
		glDetachShader(m_id, frag_shader_id);
		glDeleteShader(vertex_shader_id);
		glDeleteShader(frag_shader_id);
	}

	ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram)
	{
		m_id = shaderProgram.m_id;
		m_isCompiled = shaderProgram.m_isCompiled;

		shaderProgram.m_id = 0;
		shaderProgram.m_isCompiled = false;
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& shaderProgram)
	{
		glDeleteProgram(m_id);
		m_id = shaderProgram.m_id;
		m_isCompiled = shaderProgram.m_isCompiled;

		shaderProgram.m_id = 0;
		shaderProgram.m_isCompiled = false;
		return *this;
	}

	ShaderProgram::~ShaderProgram()
	{
		glDeleteProgram(m_id);
	}

	std::string ShaderProgram::ReadFile(const std::string& fileLocation)
	{
		// TODO using file is bad practice because of raii
		std::ifstream file(fileLocation);
		if (!file.is_open()) {
			std::string msg = "File open failed " + std::string(fileLocation);
			throw std::exception(msg.c_str());
		}

		std::stringstream stream;
		std::string str;
		while (std::getline(file, str)) {
			stream << str << '\n';;
		}
		file.close();
		return stream.str();
	}

	void ShaderProgram::bind() const
	{
		// make shader current
		glUseProgram(m_id);
	}

	void ShaderProgram::unbind()
	{
		glUseProgram(0); // means 0 shader 
	}

	GLint ShaderProgram::get_uniform_location(const char* name) const {
		return glGetUniformLocation(m_id, name);
	}
	void ShaderProgram::set_matrix4(const char* name, const glm::mat4& matrix) const {
		// get location by name, amount of args, transponse or not, pointer to data specially for glm such way
		glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void ShaderProgram::set_matrix3(const char* name, const glm::mat3& matrix) const
	{
		glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void ShaderProgram::set_int(const char* name, const int value) const {
		glUniform1i(get_uniform_location(name), value);
	}

	void ShaderProgram::set_bool(const char* name, const bool value) const {
		glUniform1i(get_uniform_location(name), value);
	}

	void ShaderProgram::set_float(const char* name, const float value) const
	{
		glUniform1f(get_uniform_location(name), value);
	}

	void ShaderProgram::set_vec3(const char* name, const glm::vec3& v) const
	{
		glUniform3f(get_uniform_location(name), v.x, v.y, v.z);
	}
}
