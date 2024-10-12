#include "SimpleEngineCore/Window.h"
#include "SimpleEngineCore/Log.h"
#include "SimpleEngineCore/Rendering/OpenGL/Renderer_OpenGL.h"
#include "SimpleEngineCore/Modules/UIModule.h"

#include <memory>

#include <GLFW/glfw3.h>

namespace SimpleEngine {

	Window::Window(std::string title, const unsigned int width, const unsigned int height)
		: m_data({ std::move(title), width, height }) {

		int resCode = init();
	}

	Window::~Window() {
		shutdown();
	}

	int Window::init() {
		LOG_INFO("Creating window {0} width size {1}x{2}", m_data.title, m_data.width, m_data.height);

		glfwSetErrorCallback([](int error_code, const char* desc)
			{
				LOG_CRIT("GLFW error: {0}", desc);
			}
		);
		if (!glfwInit())
		{
			LOG_CRIT("Can't init GLFW!");
			return -1;
		}

		/* Create a windowed mode window and its OpenGL context */
		m_pWindow = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), NULL, NULL);
		if (!m_pWindow)
		{
			LOG_CRIT("Can't create window {0}!", m_data.title);
			return -2;
		}

		if (!Renderer_OpenGL::init(m_pWindow)) {
			LOG_CRIT("Failed to init OpenGL renderer!");
			return -3;
		}

		// set data in pointer. since we need to have it from glfwSetWindowSizeCallback only using m_pWindow
		glfwSetWindowUserPointer(m_pWindow, &m_data);

		glfwSetKeyCallback(m_pWindow,
			[](GLFWwindow* pWindow, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(pWindow);
				switch (action)
				{
				case GLFW_PRESS:
				{
					EventKeyPressed event(static_cast<KeyCode>(key), false);
					data.eventCallbackFn(event);
					break;
				}
				case GLFW_RELEASE:
				{
					EventKeyReleased event(static_cast<KeyCode>(key));
					data.eventCallbackFn(event);
					break;
				}
				case GLFW_REPEAT:
				{
					EventKeyPressed event(static_cast<KeyCode>(key), true);
					data.eventCallbackFn(event);
					break;
				}
				}
			}
		);

		// We call this function if windows size changed
		glfwSetWindowSizeCallback(m_pWindow,
			[](GLFWwindow* pWindow, int width, int height)
			{
				//LOG_INFO("New window size {0}x{1}", width, height);

				// get pointer to our data 
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(pWindow);

				data.width = width;
				data.height = height;

				// collect data for event
				EventWindowResize event(width, height);

				// actually call our own callback to handle event
				data.eventCallbackFn(event);
			}
		);

		glfwSetCursorPosCallback(m_pWindow,
			[](GLFWwindow* pWindow, double x, double y)
			{
				//LOG_INFO("New cursor pos {0}x{1}", x, y);

				// get pointer to our data 
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(pWindow);

				// collect data for event
				EventMouseMoved event(x, y);

				// actually call our own callback to handle event
				data.eventCallbackFn(event);
			}
		);

		glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow* pWindow)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(pWindow);

				// collect data for event
				EventWindowClosed event;

				// actually call our own callback to handle event
				data.eventCallbackFn(event);
			}
		);

		glfwSetFramebufferSizeCallback(m_pWindow, [](GLFWwindow* pWindow, int w, int h)
			{
				Renderer_OpenGL::set_viewport(w, h);
			}
		);

		glfwSetMouseButtonCallback(m_pWindow,
			[](GLFWwindow* pWindow, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(pWindow);
				double x_pos;
				double y_pos;
				glfwGetCursorPos(pWindow, &x_pos, &y_pos);

				switch (action)
				{
				case GLFW_PRESS:
				{
					EventMouseButtonPressed event(static_cast<MouseButtonCode>(button), x_pos, y_pos);
					data.eventCallbackFn(event);
					break;
				}
				case GLFW_RELEASE:
				{
					EventMouseButtonReleased event(static_cast<MouseButtonCode>(button), x_pos, y_pos);
					data.eventCallbackFn(event);
					break;
				}
				}
			}
		);

		UIModule::on_window_create(m_pWindow);

		return 0;
	}

	void Window::on_update() {
		glfwSwapBuffers(m_pWindow);
		glfwPollEvents();
	}

	glm::vec2 Window::get_current_cursor_pos() const {
		double x_pos;
		double y_pos;
		glfwGetCursorPos(m_pWindow, &x_pos, &y_pos);
		return { x_pos, y_pos };
	}

	void Window::shutdown() {
		UIModule::on_window_close();

		// Cleanup GLFW window
		glfwDestroyWindow(m_pWindow);
		m_pWindow = nullptr; // Prevent dangling pointer

		glfwTerminate(); // Finally, terminate GLFW
	}
}