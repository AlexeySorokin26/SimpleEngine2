#include "SimpleEngineCore/Application.h"
#include "SimpleEngineCore/Window.h"
#include "SimpleEngineCore/Log.h"
#include "SimpleEngineCore/Input.h"
#include "SimpleEngineCore/Rendering/OpenGL/VertexBuffer.h"
#include "SimpleEngineCore/Rendering/OpenGL/VertexArray.h"
#include "SimpleEngineCore/Rendering/OpenGL/IndexBuffer.h"
#include "SimpleEngineCore/Rendering/OpenGL/Renderer_OpenGL.h"
#include "SimpleEngineCore/Rendering/OpenGL/ShaderProgram.h"
#include "SimpleEngineCore/MOdules/UIModule.h"

#include <glm/mat3x3.hpp>
#include <glm/trigonometric.hpp>

#include <GLFW/glfw3.h>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

namespace SimpleEngine {

	GLfloat pos_colors[] = {
		0.0f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,
		0.0f, 0.5f,  -0.5f,   0.0f, 1.0f, 1.0f,
		0.0f, -0.5f, 0.5f,    1.0f, 0.0f, 1.0f,
		0.0f, 0.5f,  0.5f,    1.0f, 0.0f, 0.0f
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
		uniform mat4 view_proj_mat;
		out vec3 color;
		void main() {
			color = vertex_color;
			gl_Position = view_proj_mat * model_mat * vec4(vertex_position, 1.0); 
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

	Application::Application() {
		LOG_INFO("Starting Application");
	}

	Application::~Application() {
		LOG_INFO("Closing Application");
	}

	int Application::start(unsigned int window_width, unsigned int window_heigth, const char* title) {
		m_pWindow = std::make_unique<Window>(title, window_width, window_heigth);
		m_event_dispatcher.add_event_listener<EventMouseMoved>(
			[](EventMouseMoved& event) {
				//LOG_INFO("[MouseMoved] Mouse moved to {0}x{1}", event.x, event.y);
			}
		);
		m_event_dispatcher.add_event_listener<EventWindowResize>(
			[](EventWindowResize& event) {
				//LOG_INFO("[WindowResized] Window resized to {0}x{1}", event.w, event.h);
			}
		);
		m_event_dispatcher.add_event_listener<EventWindowClosed>(
			[&](EventWindowClosed& event) {
				LOG_INFO("[WindowClosed]");
				m_bCloseWindow = true;
			}
		);
		m_event_dispatcher.add_event_listener<EventKeyPressed>(
			[&](EventKeyPressed& event) {
				LOG_INFO("[EventKeyPressed]");
				Input::PressKey(event.key_code);
			}
		);
		m_event_dispatcher.add_event_listener<EventKeyReleased>(
			[&](EventKeyReleased& event) {
				LOG_INFO("[EventKeyReleased]");
				Input::ReleaseKey(event.key_code);
			}
		);
		m_event_dispatcher.add_event_listener<EventMouseButtonPressed>(
			[&](EventMouseButtonPressed& event) {
				LOG_INFO("[EventMouseButtonPressed]");
				Input::PressMouseButton(event.mouse_button_code);
				on_mouse_button_event(event.mouse_button_code, event.x_pos, event.y_pos, true);
			}
		);
		m_event_dispatcher.add_event_listener<EventMouseButtonReleased>(
			[&](EventMouseButtonReleased& event) {
				LOG_INFO("[EventMouseButtonReleased]");
				Input::ReleaseMouseButton(event.mouse_button_code);
				on_mouse_button_event(event.mouse_button_code, event.x_pos, event.y_pos, false);
			}
		);
		m_pWindow->set_event_callback(
			[&](BaseEvent& event) {
				m_event_dispatcher.dispatch(event);
			});

		// Should go from this file  
		//-----------------------------------------//
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
		//-----------------------------------------//

		while (!m_bCloseWindow) {
			Renderer_OpenGL::set_clear_color(
				m_background_color[0], m_background_color[1], m_background_color[2], m_background_color[3]);
			Renderer_OpenGL::clear();

			UIModule::on_ui_draw_begin();

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

			camera.set_projection_mode(
				perspective_camera ?
				Camera::ProjectionMode::Perspective : Camera::ProjectionMode::Orthographic);
			p_shader_program->setMatrix4("view_proj_mat", camera.get_projection_matrix() * camera.get_view_matrix());

			Renderer_OpenGL::draw(*p_vao);

			//-----------------------------------------//
			bool show = true;
			UIModule::ShowExampleAppDockSpace(&show);
			ImGui::ShowDemoWindow();
			ImGui::Begin("Background Color Window");
			ImGui::ColorEdit4("Background Color", m_background_color);
			ImGui::SliderFloat3("Scale", scale, 0.f, 2.f);
			ImGui::SliderFloat("Rotate", &rotate, 0.f, 360.f);
			ImGui::SliderFloat3("Translate", translate, -10.f, 10.f);
			ImGui::SliderFloat3("Cam pos", camera_pos, -10.f, 10.f);
			ImGui::SliderFloat3("Cam rotation", camera_rotation, 0.f, 360);
			ImGui::Checkbox("Perspective camera", &perspective_camera);
			ImGui::End();
			//-----------------------------------------//

			on_ui_draw();
			UIModule::on_ui_draw_end();

			m_pWindow->on_update();
			on_update();
		}

		return 0;
	}

	glm::vec2 Application::get_current_cursor_pos() const {
		return m_pWindow->get_current_cursor_pos();
	}

}