#include "ShaderProgram.h"

#include "SimpleEngineCore/Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

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
	SimpleEngine::ShaderProgram::ShaderProgram(const char* vertex_shader_src, const char* frag_shader_src)
	{
		GLuint vertex_shader_id = 0;
		if (!create_shader(vertex_shader_src, GL_VERTEX_SHADER, vertex_shader_id)) {
			LOG_CRIT("Vertex shader: compile-time error!");
			glDeleteShader(vertex_shader_id);
			return;
		}

		GLuint frag_shader_id = 0;
		if (!create_shader(frag_shader_src, GL_FRAGMENT_SHADER, frag_shader_id)) {
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

	SimpleEngine::ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram)
	{
		m_id = shaderProgram.m_id;
		m_isCompiled = shaderProgram.m_isCompiled;

		shaderProgram.m_id = 0;
		shaderProgram.m_isCompiled = false;
	}

	ShaderProgram& SimpleEngine::ShaderProgram::operator=(ShaderProgram&& shaderProgram)
	{
		glDeleteProgram(m_id);
		m_id = shaderProgram.m_id;
		m_isCompiled = shaderProgram.m_isCompiled;

		shaderProgram.m_id = 0;
		shaderProgram.m_isCompiled = false;
		return *this;
	}

	SimpleEngine::ShaderProgram::~ShaderProgram()
	{
		glDeleteProgram(m_id);
	}

	void SimpleEngine::ShaderProgram::bind() const
	{
		// make shader current
		glUseProgram(m_id);
	}

	void SimpleEngine::ShaderProgram::unbind()
	{
		glUseProgram(0); // means 0 shader 
	}
}
