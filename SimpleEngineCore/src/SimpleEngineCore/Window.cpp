#include "SimpleEngineCore/Window.h"
#include "SimpleEngineCore/Log.h"
#include "SimpleEngineCore/Rendering/OpenGL/ShaderProgram.h"
#include "SimpleEngineCore/Rendering/OpenGL/VertexBuffer.h"
#include "SimpleEngineCore/Rendering/OpenGL/VertexArray.h"
#include "SimpleEngineCore/Rendering/OpenGL/IndexBuffer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <memory>

#include <glm/mat3x3.hpp>
#include <glm/trigonometric.hpp>

namespace SimpleEngine {

	static bool s_GLFW_initialized = false;

	GLfloat pos_colors[] = {
	   -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 1.0f,
	   -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f,
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f
	};

	GLuint indices[] = {
		0, 1, 2, 3, 2, 1
	};

	// location is a position where shader should search data like pos in function when we are passing data
	const char* vertex_shader =
		R"(#version 460
		layout(location=0) in vec3 vertex_position; 
		layout(location=1) in vec3 vertex_color;
		uniform mat4 model_mat;
		out vec3 color;
		void main() {
			color = vertex_color;
			gl_Position = model_mat * vec4(vertex_position, 1.0); 
		})";

	const char* frag_shader =
		R"(#version 460
		in vec3 color;
		out vec4 frag_color;
		void main() {
			frag_color = vec4(color, 1.0);
		})";

	float m_background_color[] = { 0, 0, 0, 1 };

	std::unique_ptr<ShaderProgram> p_shader_program;
	std::unique_ptr<VertexBuffer> p_pos_colors_vbo;
	std::unique_ptr<IndexBuffer> p_index_buffer;
	std::unique_ptr<VertexArray> p_vao;
	float scale[3] = { 1.f, 1.f, 1.f };
	float rotate = 0.f;
	float translate[3] = { 0.f, 0.f, 0.f };

	Window::Window(std::string title, const unsigned int width, const unsigned int height)
		: m_data({ std::move(title), width, height }) {

		int resCode = init();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplOpenGL3_Init();
		ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);
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
				//LOG_INFO("New buffer size {0}x{1}", w, h);
				glViewport(0, 0, w, h);
			}
		);

		// First we create shader programms
		p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, frag_shader);
		if (!p_shader_program->isCompiled())
			return false;

		BufferLayout buffer_layout_2vec3
		{
			ShaderDataType::Float3,
			ShaderDataType::Float3
		};

		// NOW we have to PASS our CPU data in shaders 
		// using VERTEX BUFFER OBJECT to allocate and fill memory on gpu
		p_vao = std::make_unique<VertexArray>();
		p_pos_colors_vbo = std::make_unique<VertexBuffer>(pos_colors, sizeof(pos_colors), buffer_layout_2vec3);
		p_index_buffer = std::make_unique<IndexBuffer>(indices, sizeof(indices) / sizeof(GLuint));

		// NEXT BIND data from buffers with our shaders
		// using VERTEX ARRAY OBJECT
		p_vao->add_vertex_buffer(*p_pos_colors_vbo);
		p_vao->set_index_buffer(*p_index_buffer);

		return 0;
	}

	void Window::on_update() {
		glClearColor(m_background_color[0], m_background_color[1], m_background_color[2], m_background_color[3]);
		glClear(GL_COLOR_BUFFER_BIT);

		// set window size where imgui should draw
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = static_cast<float>(get_width());
		io.DisplaySize.y = static_cast<float>(get_height());

		// create frame for imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Background Color Window");
		ImGui::ColorEdit4("Background Color", m_background_color);
		ImGui::SliderFloat3("Scale", scale, 0.f, 2.f);
		ImGui::SliderFloat("Rotate", &rotate, 0.f, 360.f);
		ImGui::SliderFloat3("Translate", translate, -10.f, 10.f);

		p_shader_program->bind();

		glm::mat4 scale_mat(
			scale[0], 0, 0, 0,
			0, scale[1], 0, 0,
			0, 0, scale[2], 0,
			0, 0, 0, 1);

		float rotate_in_rad = glm::radians(rotate);
		// here its colum major so turned a bit 
		glm::mat4 rotate_mat(
			cos(rotate_in_rad), sin(rotate_in_rad), 0, 0,
			-sin(rotate_in_rad), cos(rotate_in_rad), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

		glm::mat4 translate_mat(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			translate[0], translate[1], translate[2], 1);
		glm::mat4 model_mat = translate_mat * rotate_mat * scale_mat;
		p_shader_program->setMatrix4("model_mat", model_mat);

		p_vao->bind();
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(p_vao->get_indices_count()), GL_UNSIGNED_INT, nullptr);

		ImGui::End();

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