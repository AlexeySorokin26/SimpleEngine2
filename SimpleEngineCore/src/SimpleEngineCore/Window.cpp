#include "SimpleEngineCore/Window.h"
#include "SimpleEngineCore/Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace SimpleEngine {

	static bool s_GLFW_initialized = false;

	Window::Window(std::string title, const unsigned int width, const unsigned int height)
		: m_data({ std::move(title), width, height }) {

		int resCode = init();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplOpenGL3_Init();
	}
	Window::~Window() {
		shutdown();
	}

	int Window::init() {
		LOG_INFO("Creating window {0} width size {1}x{2}", m_data.title, m_data.width, m_data.height);

		if (!s_GLFW_initialized) {
			if (!glfwInit())
			{
				LOG_CRIT("Can't init GLFW!");
				return -1;
			}
			s_GLFW_initialized = true;
		}

		/* Create a windowed mode window and its OpenGL context */
		m_pWindow = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), NULL, NULL);
		if (!m_pWindow)
		{
			LOG_CRIT("Can't create window {0}!", m_data.title);
			glfwTerminate();
			return -2;
		}

		/* Make the window's context current */
		glfwMakeContextCurrent(m_pWindow);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			LOG_CRIT("Failed to init GLAD");
			return -3;
		}

		// set data in pointer. since we need to have it from glfwSetWindowSizeCallback only using m_pWindow
		glfwSetWindowUserPointer(m_pWindow, &m_data);

		// We call this function if windows size changed
		glfwSetWindowSizeCallback(m_pWindow,
			[](GLFWwindow* pWindow, int width, int height)
			{
				LOG_INFO("New window size {0}x{1}", width, height);

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
				LOG_INFO("New cursor pos {0}x{1}", x, y);

				// get pointer to our data 
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(pWindow);

				// collect data for event
				EventMouseMoved event(x, y);

				// actually call our own callback to handle event
				data.eventCallbackFn(event);
			}
		);
		glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow* pWindow) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(pWindow);

			// collect data for event
			EventWindowClosed event;

			// actually call our own callback to handle event
			data.eventCallbackFn(event);
			}
		);
		return 0;
	}

	void Window::on_update() {
		glClearColor(1, 1, 1, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		// set window size where imgui should draw
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = static_cast<float>(get_width());
		io.DisplaySize.y = static_cast<float>(get_height());

		// create frame for imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		// render it in imgui
		ImGui::Render();
		// use opengl to draw
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_pWindow);
		glfwPollEvents();
	}

	void Window::shutdown() {
		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}

}