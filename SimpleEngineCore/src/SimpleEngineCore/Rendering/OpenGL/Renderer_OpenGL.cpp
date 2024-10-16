#include "Renderer_OpenGL.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VertexArray.h"
#include "SimpleEngineCore/Log.h"

namespace SimpleEngine {
	bool Renderer_OpenGL::init(GLFWwindow* pWindow)
	{
		/* Make the window's context current */
		glfwMakeContextCurrent(pWindow);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			LOG_CRIT("Failed to init GLAD");
			return false;
		}
		return true;
	}
	void Renderer_OpenGL::draw(const VertexArray& v_arr)
	{
		v_arr.bind();
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(v_arr.get_indices_count()), GL_UNSIGNED_INT, nullptr);

	}
	void Renderer_OpenGL::set_clear_color(const float r, const float g, const float b, const float a)
	{
		glClearColor(r, g, b, a);
	}
	void Renderer_OpenGL::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void Renderer_OpenGL::enable_depth_testing()
	{
		glEnable(GL_DEPTH_TEST);
	}
	void Renderer_OpenGL::disable_depth_testing()
	{
		glDisable(GL_DEPTH_TEST);
	}
	void Renderer_OpenGL::set_viewport(const unsigned int w, const unsigned int h, const unsigned int left_offset, const unsigned int bottom_offset)
	{
		glViewport(left_offset, bottom_offset, w, h);
	}
	const char* Renderer_OpenGL::get_vendor_str()
	{
		return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	}
	const char* Renderer_OpenGL::get_renderer_str()
	{
		return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	}
	const char* Renderer_OpenGL::get_version_str()
	{
		return reinterpret_cast<const char*>(glGetString(GL_VERSION));
	}
}