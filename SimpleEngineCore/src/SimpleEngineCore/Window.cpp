#include "SimpleEngineCore/Window.h"
#include "SimpleEngineCore/Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace SimpleEngine {

	static bool s_GLFW_initialized = false;

	Window::Window(std::string title, const unsigned int width, const unsigned int height) :
		m_title(std::move(title)),
		m_width(width),
		m_height(height) {
		int resCode = init();
	}
	Window::~Window() {
		shutdown();
	}

	int Window::init() {
		LOG_INFO("Creating window {0} width size {1}x{2}", m_title, m_width, m_height);

		if (!s_GLFW_initialized) {
			if (!glfwInit())
			{
				LOG_CRIT("Can't init GLFW!");
				return -1;
			}
			s_GLFW_initialized = true;
		}


		/* Create a windowed mode window and its OpenGL context */
		m_pWindow = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
		if (!m_pWindow)
		{
			LOG_CRIT("Can't create window {0}!", m_title);
			glfwTerminate();
			return -2;
		}

		/* Make the window's context current */
		glfwMakeContextCurrent(m_pWindow);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			LOG_CRIT("Failed to init GLAD");
			return -3;
		}

		return 0;
	}


	void Window::on_update() {
		glClearColor(1, 1, 1, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(m_pWindow);
		glfwPollEvents();
	}

	void Window::shutdown() {
		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}

}