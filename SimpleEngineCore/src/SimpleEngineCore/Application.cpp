#define STB_IMAGE_IMPLEMENTATION


#include "SimpleEngineCore/Application.h"
#include "SimpleEngineCore/Window.h"
#include "SimpleEngineCore/Log.h"
#include "SimpleEngineCore/Input.h"
#include "SimpleEngineCore/Rendering/OpenGL/VertexBuffer.h"
#include "SimpleEngineCore/Rendering/OpenGL/VertexArray.h"
#include "SimpleEngineCore/Rendering/OpenGL/IndexBuffer.h"
#include "SimpleEngineCore/Rendering/OpenGL/Renderer_OpenGL.h"
#include "SimpleEngineCore/Rendering/OpenGL/ShaderProgram.h"
#include "SimpleEngineCore/Rendering/OpenGL/Texture2D.h"
#include "SimpleEngineCore/Rendering/OpenGL/Mesh.h"
#include "SimpleEngineCore/Modules/UIModule.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <filesystem>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>


namespace SimpleEngine {
	std::unique_ptr<Cube> cube;
	std::unique_ptr<LightCube> lightCube;
	std::unique_ptr<Cube> groundCube;

	Application::Application() {
		LOG_INFO("Starting Application");
	}

	Application::~Application() {
		LOG_INFO("Closing Application");
	}

	int Application::start(unsigned int window_width, unsigned int window_heigth, const char* title) {
		m_pWindow = std::make_unique<Window>(title, window_width, window_heigth);
		camera.set_viewport_size(static_cast<float>(window_width), static_cast<float>(window_heigth));
		m_event_dispatcher.add_event_listener<EventMouseMoved>(
			[](EventMouseMoved& event) {
				//LOG_INFO("[MouseMoved] Mouse moved to {0}x{1}", event.x, event.y);
			}
		);
		m_event_dispatcher.add_event_listener<EventWindowResize>(
			[&](EventWindowResize& event) {
				LOG_INFO("[WindowResized] Window resized to {0}x{1}", event.w, event.h);
				camera.set_viewport_size(static_cast<float>(event.w), static_cast<float>(event.h));

				draw();
			}
		);
		m_event_dispatcher.add_event_listener<EventWindowClosed>(
			[&](EventWindowClosed& event) {
				LOG_INFO("[WindowClosed]");
				close();
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

		m_event_dispatcher.add_event_listener<EventMouseScroll>(
			[&](EventMouseScroll& event) {
				LOG_INFO("[EventMouseScroll] xoffset{0} and yoffset{1}", event.xoffset, event.yoffset);
				scroll = true;
				if (event.yoffset > 0)
					scrollUp = true;
				else
					scrollUp = false;
			}
		);

		m_pWindow->set_event_callback(
			[&](BaseEvent& event) {
				m_event_dispatcher.dispatch(event);
			});

		std::filesystem::path cubeTexturePath = getBasePath() / "textures" / "brick.png";
		std::filesystem::path groundCubeTexturePath = getBasePath() / "textures" / "dirt.png";
		cube = std::make_unique<Cube>(
			Material(),
			glm::vec3{ -2.f, -2.f, 4.f },
			cubeTexturePath.string(),
			groundCubeTexturePath.string()
		);

		lightCube = std::make_unique<LightCube>();

		Material groundCubeMat = Material();
		groundCubeMat.color[0] = 0.224f;
		groundCubeMat.color[1] = 0.400f;
		groundCubeMat.color[2] = 0.271f;
		groundCubeMat.color[3] = 1.0f;

		groundCube = std::make_unique<Cube>(
			groundCubeMat,
			glm::vec3{ 0,0,-2 },
			groundCubeTexturePath.string()
		);

		Renderer_OpenGL::enable_depth_testing();
		while (!m_bCloseWindow) {
			draw();
		}

		// clean up
		m_pWindow = nullptr;
		return 0;
	}

	glm::vec2 Application::get_current_cursor_pos() const {
		return m_pWindow->get_current_cursor_pos();
	}

	void Application::draw()
	{
		Renderer_OpenGL::set_clear_color(
			m_background_color[0], m_background_color[1], m_background_color[2], m_background_color[3]);
		Renderer_OpenGL::clear();

		cube->draw(camera, light_source_color, light_source_pos, glm::vec3(scale_factor));
		lightCube->draw(camera, light_source_color, light_source_pos);
		groundCube->draw(camera, light_source_color, light_source_pos, glm::vec3{ 50, 50, 1 });

		UIModule::on_ui_draw_begin();
		on_ui_draw();
		UIModule::on_ui_draw_end();

		m_pWindow->on_update();
		on_update();
	}

}