#pragma once

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
		bool isCompiled() const { return m_isCompiled; }
	
	private:
		bool m_isCompiled = false;
		unsigned int m_id = 0;
	};
}