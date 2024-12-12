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
	std::unique_ptr<Model> model;

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

		DirectionalLight dirLight(
			directional_light_direction, light_ambient_factor, light_diffuse_factor, light_specular_factor,
			light_ambient_intensity, light_diffuse_intensity, light_specular_intensity);
		PointLight pointLight(
			point_light_position, light_ambient_factor, light_diffuse_factor, light_specular_factor,
			light_ambient_intensity, light_diffuse_intensity, light_specular_intensity);
		SpotLight spotLight(camera.get_camera_pos(), camera.get_camera_direction(),
			light_ambient_factor, light_diffuse_factor, light_specular_factor,
			light_ambient_intensity, light_diffuse_intensity, light_specular_intensity);

		// Textures paths
		std::filesystem::path cubeDiffuseTexturePath = getBasePath() / "textures" / "material.diffuse.png";
		std::filesystem::path cubeSpecularTexturePath = getBasePath() / "textures" / "material.specular.png";
		std::vector<std::filesystem::path> v_texturePaths;
		v_texturePaths.push_back(cubeDiffuseTexturePath);
		v_texturePaths.push_back(cubeSpecularTexturePath);
		// Cube with 2 textures
		{
			std::filesystem::path shaderPath = getBasePath() / "shaders";
			std::filesystem::path vertex_shader_path = shaderPath / "phong_cube_vertex_shader.glsl";
			std::filesystem::path frag_shader_path = shaderPath / "phong_cube_fragment_shader.glsl";

			cube = std::make_unique<Cube>(
				Material(),
				vertex_shader_path,
				frag_shader_path,
				glm::vec3{ -2.f, -2.f, 4.f },
				v_texturePaths,
				verticesCube,
				indicesCube,
				camera,
				dirLight, useDirectionalLight,
				pointLight, usePointLight,
				spotLight, useSpotLight,
				glm::vec3(0), glm::vec3(cube_scale_factor)
			);
		}

		// Directional light cube (to immitate it)
		{
			std::filesystem::path shaderPath = getBasePath() / "shaders";
			std::filesystem::path vertex_shader_path = shaderPath / "phong_cube_vertex_shader.glsl";
			std::filesystem::path frag_shader_path = shaderPath / "phong_cube_fragment_shader.glsl";
			std::vector<std::filesystem::path> v_texturePathsEmpty;
			directionalLightCube = std::make_unique<Cube>(
				Material(glm::vec3(1), 0),
				vertex_shader_path,
				frag_shader_path,
				glm::vec3{ 0.f, 0.f, 10.f },
				v_texturePathsEmpty,
				verticesCube,
				indicesCube,
				camera,
				dirLight, false,
				pointLight, false,
				spotLight, false,
				dirLight.direction, glm::vec3(0.1, 0.1, 0.5)
			);
		}

		// Light cube 
		{
			std::filesystem::path shaderPath = getBasePath() / "shaders";
			std::filesystem::path vertex_shader_path = shaderPath / "light_cube_vertex_shader.glsl";
			std::filesystem::path frag_shader_path = shaderPath / "light_cube_fragment_shader.glsl";
			lightCube = std::make_unique<LightCube>(
				verticesCube, indicesCube,
				vertex_shader_path, frag_shader_path,
				camera, pointLight
			);
		}

		// Ground cube 
		{
			std::filesystem::path shaderPath = getBasePath() / "shaders";
			std::filesystem::path vertex_shader_path = shaderPath / "phong_cube_vertex_shader.glsl";
			std::filesystem::path frag_shader_path = shaderPath / "phong_cube_fragment_shader.glsl";
			//std::filesystem::path groundCubeTexturePath = getBasePath() / "textures" / "dirt.png";
			//std::vector<std::filesystem::path> v_texturePaths;
			groundCube = std::make_unique<Cube>(
				Material(),
				vertex_shader_path,
				frag_shader_path,
				glm::vec3{ 0,0,-2 },
				v_texturePaths,
				verticesCube,
				indicesCube, camera,
				dirLight, useDirectionalLight,
				pointLight, usePointLight,
				spotLight, useSpotLight,
				glm::vec3(0), glm::vec3{ 50, 50, 1 }
			);
		}

		// Model testing
		{
			std::filesystem::path shaderPath = getBasePath() / "shaders";
			std::filesystem::path vertex_shader_path = shaderPath / "light_cube_vertex_shader.glsl";
			std::filesystem::path frag_shader_path = shaderPath / "light_cube_fragment_shader.glsl";
			std::filesystem::path modelPath = getBasePath() / "models/Cake";
			model = std::make_unique<Model>(
				MeshType::LightCube, modelPath / "cake.obj",
				vertex_shader_path, frag_shader_path
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
		SpotLight spotLight(camera.get_camera_pos(), camera.get_camera_direction(),
			light_ambient_factor, light_diffuse_factor, light_specular_factor,
			light_ambient_intensity, light_diffuse_intensity, light_specular_intensity);

		if (camera.if_update_view_matrix()) {
			camera.update_view_matrix();
			camera.set_update_view_matirx(false);
		}

		cube->UpdateLight(
			dirLight, useDirectionalLight,
			pointLight, usePointLight,
			spotLight, useSpotLight
		);
		cube->UpdateCamera(camera);
		cube->Draw();

		/*directionalLightCube->UpdateDirVector(dirLight.direction);
		directionalLightCube->UpdateCamera(camera);
		directionalLightCube->Draw();

		lightCube->UpdateLight(pointLight);
		lightCube->UpdateCamera(camera);
		lightCube->Draw();
		*/

		groundCube->UpdateLight(
			dirLight, useDirectionalLight,
			pointLight, usePointLight,
			spotLight, useSpotLight
		);
		groundCube->UpdateCamera(camera);
		groundCube->Draw();

		model->UpdateLight(pointLight);
		model->UpdateCamera(camera);
		model->Draw();

		UIModule::on_ui_draw_begin();
		on_ui_draw();
		UIModule::on_ui_draw_end();

		m_pWindow->on_update();
		on_update();
	}

}