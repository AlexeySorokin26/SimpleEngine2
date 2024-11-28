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

#include <glm/vec3.hpp>

std::vector<GLfloat> verticesCube = {
	//    position             normal            UV                  index
	// FRONT
	-1.0f, -1.f, -1.f,    -1.f,  0.f,  0.f,     0.f, 0.f,              // 0
	-1.0f,  1.f, -1.f,    -1.f,  0.f,  0.f,     1.f, 0.f,              // 1
	-1.0f,  1.f,  1.f,    -1.f,  0.f,  0.f,     1.f, 1.f,              // 2
	-1.0f, -1.f,  1.f,    -1.f,  0.f,  0.f,     0.f, 1.f,              // 3
	// BACK                                  
	 1.0f, -1.f, -1.f,     1.f,  0.f,  0.f,     1.f, 0.f,              // 4
	 1.0f,  1.f, -1.f,     1.f,  0.f,  0.f,     0.f, 0.f,              // 5
	 1.0f,  1.f,  1.f,     1.f,  0.f,  0.f,     0.f, 1.f,              // 6
	 1.0f, -1.f,  1.f,     1.f,  0.f,  0.f,     1.f, 1.f,              // 7
	 // RIGHT
	 -1.0f,  1.f, -1.f,     0.f,  1.f,  0.f,     0.f, 0.f,              // 8
	  1.0f,  1.f, -1.f,     0.f,  1.f,  0.f,     1.f, 0.f,              // 9
	  1.0f,  1.f,  1.f,     0.f,  1.f,  0.f,     1.f, 1.f,              // 10
	 -1.0f,  1.f,  1.f,     0.f,  1.f,  0.f,     0.f, 1.f,              // 11
	 // LEFT
	 -1.0f, -1.f, -1.f,     0.f, -1.f,  0.f,     1.f, 0.f,              // 12
	  1.0f, -1.f, -1.f,     0.f, -1.f,  0.f,     0.f, 0.f,              // 13
	  1.0f, -1.f,  1.f,     0.f, -1.f,  0.f,     0.f, 1.f,              // 14
	 -1.0f, -1.f,  1.f,     0.f, -1.f,  0.f,     1.f, 1.f,              // 15
	 // TOP
	 -1.0f, -1.f,  1.f,     0.f,  0.f,  1.f,     0.f, 0.f,              // 16
	 -1.0f,  1.f,  1.f,     0.f,  0.f,  1.f,     1.f, 0.f,              // 17
	  1.0f,  1.f,  1.f,     0.f,  0.f,  1.f,     1.f, 1.f,              // 18
	  1.0f, -1.f,  1.f,     0.f,  0.f,  1.f,     0.f, 1.f,              // 19
	  // BOTTOM
	  -1.0f, -1.f, -1.f,    0.f,  0.f, -1.f,     0.f, 1.f,              // 20
	  -1.0f,  1.f, -1.f,    0.f,  0.f, -1.f,     1.f, 1.f,              // 21
	   1.0f,  1.f, -1.f,    0.f,  0.f, -1.f,     1.f, 0.f,              // 22
	   1.0f, -1.f, -1.f,    0.f,  0.f, -1.f,     0.f, 0.f,              // 23
};

std::vector<GLuint> indicesCube = {
	0,   1,  2,  2,  3,  0, // front
	4,   5,  6,  6,  7,  4, // back
	8,   9, 10, 10, 11,  8, // right
	12, 13, 14, 14, 15, 12, // left
	16, 17, 18, 18, 19, 16, // top
	20, 21, 22, 22, 23, 20  // bottom
};


namespace SimpleEngine {
	std::unique_ptr<Cube> cube;
	std::unique_ptr<LightCube> lightCube;
	std::unique_ptr<Cube> groundCube;

	std::unique_ptr<Cube> directionalLightCube;

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

		// Textures paths
		std::filesystem::path cubeDiffuseTexturePath = getBasePath() / "textures" / "containerDiffuse.png";
		std::filesystem::path cubeSpecularTexturePath = getBasePath() / "textures" / "containerSpecular.png";
		std::filesystem::path groundCubeTexturePath = getBasePath() / "textures" / "dirt.png";
		// Cube with 2 textures
		{
			cube = std::make_unique<Cube>(
				Material(),
				glm::vec3{ -2.f, -2.f, 4.f },
				cubeDiffuseTexturePath.string(),
				cubeSpecularTexturePath.string(),
				verticesCube,
				indicesCube
			);
		}

		// Directional light cube (to immitate it)
		{
			directionalLightCube = std::make_unique<Cube>(
				Material(glm::vec3(1), 0),
				glm::vec3{ 0.f, 0.f, 10.f },
				"",
				"",
				verticesCube,
				indicesCube
			);
		}

		// Light cube 
		{
			lightCube = std::make_unique<LightCube>(verticesCube, indicesCube);
		}

		// Ground cube 
		{
			groundCube = std::make_unique<Cube>(
				Material(),
				glm::vec3{ 0,0,-2 },
				groundCubeTexturePath.string(),
				"",
				verticesCube,
				indicesCube
			);
		}

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

		DirectionalLight dirLight(
			directional_light_direction, light_ambient_factor, light_diffuse_factor, light_specular_factor,
			light_ambient_intensity, light_diffuse_intensity, light_specular_intensity);
		PointLight pointLight(
			point_light_position, light_ambient_factor, light_diffuse_factor, light_specular_factor,
			light_ambient_intensity, light_diffuse_intensity, light_specular_intensity);
		cube->draw(camera,
			dirLight, useDirectionalLight,
			pointLight, usePointLight,
			useSpotLight,
			glm::vec3(0), glm::vec3(cube_scale_factor)
		);
		directionalLightCube->draw(camera,
			dirLight, false,
			pointLight, false,
			false,
			dirLight.direction, glm::vec3(0.1, 0.1, 0.5)
		);
		lightCube->draw(camera, pointLight);
		groundCube->draw(camera,
			dirLight, useDirectionalLight,
			pointLight, usePointLight,
			useSpotLight,
			glm::vec3(0), glm::vec3{ 50, 50, 1 }
		);

		UIModule::on_ui_draw_begin();
		on_ui_draw();
		UIModule::on_ui_draw_end();

		m_pWindow->on_update();
		on_update();
	}

}