#include "SimpleEngineCore/Window.h"
#include "SimpleEngineCore/Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace SimpleEngine {

	static bool s_GLFW_initialized = false;

	GLfloat points[] = {
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
	};

	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		0.5f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};

	const char* vertex_shader =
		"#version 460\n"
		"layout(location=0) in vec3 vertex_position;" // location is a position where shader should search data like pos in function when we are passing data
		"layout(location=1) in vec3 vertex_color;"
		"out vec3 color;"
		"void main() {"
		"	color = vertex_color;"
		"	gl_Position = vec4(vertex_position, 1.0);" // normalized setted inner position of vertex (out position of vertex)
		"}";

	const char* frag_shader =
		"#version 460\n"
		"in vec3 color;"
		"out vec4 frag_color;"
		"void main() {"
		"	frag_color = vec4(color, 1.0);"
		"}";

	GLuint shader_program;
	GLuint vao;

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

		glfwSetFramebufferSizeCallback(m_pWindow, [](GLFWwindow* pWindow, int w, int h)
			{
				LOG_INFO("New buffer size {0}x{1}", w, h);
				glViewport(0, 0, w, h);
			}
		);

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vertex_shader, nullptr);
		glCompileShader(vs);

		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &frag_shader, nullptr);
		glCompileShader(fs);

		shader_program = glCreateProgram();
		glAttachShader(shader_program, vs); // link vs
		glAttachShader(shader_program, fs); // link fs
		glLinkProgram(shader_program); // create final programm

		glDeleteShader(vs);
		glDeleteShader(fs);

		// NOW we have to PASS our data in shaders 
		// using VERTEX BUFFER OBJECT to allocate and fill memory on gpu
		GLuint points_vbo = 0;
		// gen buffer and write its id in points_vbo adress
		glGenBuffers(1, &points_vbo);
		// connect buffer and make it current (only one can be current)
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
		// fill buffer by transfering data from cpu to gpu memory
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW); // static means data in buffer do not change

		// the same for colors
		GLuint colors_vbo = 0;
		glGenBuffers(1, &colors_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

		// NEXT we have to BIND data from buffers with our shaders 
		// basicly tell gpu how to manage data 
		// for that we are using VERTEX ARRAY OBJECT
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		// now bind our VBO with its position in vertex shaders 
		// first we have to TURN ON position 
		glEnableVertexAttribArray(0);
		// again have to make our vbo for points first active since currently for colors is active
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
		// finally link data: position, amount of data, type, norm, stride, shift
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		// the same for colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		return 0;
	}

	void Window::on_update() {
		glClearColor(1, 1, 1, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		// attach shader we wwant to use it for every vertex and fragment
		glUseProgram(shader_program);
		// attach vao
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

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