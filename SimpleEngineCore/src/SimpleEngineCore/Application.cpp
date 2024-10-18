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
#include "SimpleEngineCore/MOdules/UIModule.h"

#include <glm/mat3x3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

namespace SimpleEngine {

	//GLfloat pos_colors[] = {
	//	// vertex coord (X,Y,Z)  // color rgb           // texture (T,S)
	//	0.0f, -0.5f, -0.5f,      1.0f, 1.0f, 0.0f,		2.f, -1.f,
	//	0.0f, 0.5f,  -0.5f,      0.0f, 1.0f, 1.0f,		-1.f, -1.f,
	//	0.0f, -0.5f, 0.5f,       1.0f, 0.0f, 1.0f,		2.f, 2.f,
	//	0.0f, 0.5f,  0.5f,       1.0f, 0.0f, 0.0f,		-1.f, 2.f
	//};

	GLfloat pos_norm_uv[] = {
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

	GLuint indices[] = {
		0,   1,  2,  2,  3,  0, // front
		4,   5,  6,  6,  7,  4, // back
		8,   9, 10, 10, 11,  8, // right
		12, 13, 14, 14, 15, 12, // left
		16, 17, 18, 18, 19, 16, // top
		20, 21, 22, 22, 23, 20  // bottom
	};

	void generate_circle(unsigned char* data,
		const unsigned int width,
		const unsigned int height,
		const unsigned int center_x,
		const unsigned int center_y,
		const unsigned int radius,
		const unsigned char color_r,
		const unsigned char color_g,
		const unsigned char color_b)
	{
		for (unsigned int y = 0; y < height; ++y)
		{
			for (unsigned int x = 0; x < width; ++x)
			{
				if ((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y) < radius * radius)
				{
					// shift computed in bytes not in pixels
					data[3 * (x + width * y) + 0] = color_r;
					data[3 * (x + width * y) + 1] = color_g;
					data[3 * (x + width * y) + 2] = color_b;
				}
			}
		}
	}

	void generate_smile_texture(unsigned char* data,
		const unsigned int width,
		const unsigned int height)
	{
		// background
		for (unsigned int x = 0; x < width; ++x)
		{
			for (unsigned int y = 0; y < height; ++y)
			{
				data[3 * (x + width * y) + 0] = 200;
				data[3 * (x + width * y) + 1] = 191;
				data[3 * (x + width * y) + 2] = 231;
			}
		}
		// face
		generate_circle(data, width, height, width * 0.5, height * 0.5, width * 0.4, 255, 255, 0);
		// smile
		generate_circle(data, width, height, width * 0.5, height * 0.4, width * 0.2, 0, 0, 0);
		generate_circle(data, width, height, width * 0.5, height * 0.45, width * 0.2, 255, 255, 0);
		// eyes
		generate_circle(data, width, height, width * 0.35, height * 0.6, width * 0.07, 255, 0, 255);
		generate_circle(data, width, height, width * 0.65, height * 0.6, width * 0.07, 0, 0, 255);
	}

	void generate_quads_texture(
		unsigned char* data,
		const unsigned int width,
		const unsigned int height)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			for (unsigned int y = 0; y < height; ++y)
			{
				if ((x < width / 2 && y < height / 2) || x >= width / 2 && y >= height / 2)
				{
					data[3 * (x + width * y) + 0] = 0;
					data[3 * (x + width * y) + 1] = 0;
					data[3 * (x + width * y) + 2] = 0;
				}
				else
				{
					data[3 * (x + width * y) + 0] = 255;
					data[3 * (x + width * y) + 1] = 255;
					data[3 * (x + width * y) + 2] = 255;
				}
			}
		}
	}

	// location is a position where shader should search data like pos in function when we are passing data
	const char* vertex_shader =
		R"(
		#version 460

		layout(location=0) in vec3 vertex_position; 
		layout(location=1) in vec3 vertex_normal; 
		layout(location=2) in vec2 texture_coord;

		uniform mat4 model_view_mat; // go into camera space
		uniform mat4 mvp_mat; // model * view * proj mat
		uniform mat3 normal_matrix;	// mat for normal; it has to be slightly different from our mv matrix because of transforamtion of space and normals should be transformed a bit differently from 38 lecture SimpleCoding 	 
		uniform int current_frame;

		out vec2 tex_coord_smile;
		out vec2 tex_coord_quads;
		out vec3 frag_normal_eye; // in camera space
		out vec3 frag_pos_eye; // in camera space

		void main() {
			tex_coord_smile = texture_coord;
			tex_coord_quads = texture_coord + vec2(current_frame / 1000.f, current_frame / 1000.f);
			
			frag_normal_eye = normal_matrix * vertex_normal;
			frag_pos_eye = vec3(model_view_mat * vec4(vertex_position, 1.0));
			gl_Position = mvp_mat * vec4(vertex_position, 1.0);
		})";

	const char* frag_shader =
		R"(
		#version 460

		in vec2 tex_coord_smile;
		in vec2 tex_coord_quads;
		in vec3 frag_pos_eye; // now we got data in cam space 
		in vec3 frag_normal_eye;
		
		layout(binding=0) uniform sampler2D InTexture_Smile;
		layout(binding=1) uniform sampler2D InTexture_Quads;

		uniform vec3 light_color;
		uniform vec3 light_pos_eye;
		uniform float ambient_factor;
		uniform float diffuse_factor;
		uniform float specular_factor;
		uniform float shininess;

		out vec4 frag_color;

		void main() {
			//frag_color = texture(InTexture_Smile, tex_coord_smile) * texture(InTexture_Quads, tex_coord_quads);
			
			// ambient
			vec3 ambient = ambient_factor * light_color;
			
			// diffuse
			vec3 normal = normalize(frag_normal_eye);
			vec3 light_direction = normalize(light_pos_eye-frag_pos_eye);
			vec3 diffuse = diffuse_factor * light_color * max(dot(normal, light_direction), 0);
			
			// specular 
			vec3 view_dir = normalize(-frag_pos_eye); // vector to cam
			vec3 reflect_dir = reflect(-light_direction, normal);
			vec3 specular = specular_factor * light_color * pow(max(dot(view_dir, reflect_dir), 0), shininess);
	
			frag_color = vec4((ambient + diffuse + specular), 1.f) * texture(InTexture_Smile, tex_coord_smile);
		})";

	const char* light_source_vertex_shader =
		R"(
		#version 460

		layout(location=0) in vec3 vertex_position; 
		layout(location=1) in vec3 vertex_normal; 
		layout(location=2) in vec2 texture_coord;

		uniform mat4 mvp_mat;

		void main() {
			gl_Position = mvp_mat * vec4(vertex_position * 0.1f, 1.0); 
		})";

	const char* light_source_frag_shader =
		R"(
		#version 460		
		out vec4 frag_color;

		uniform vec3 light_color;

		void main() {
				frag_color = vec4(light_color, 1.0);
		})";

	float m_background_color[] = { 0, 0, 0, 1 };

	std::array<glm::vec3, 5> positions = {
			glm::vec3(-2.f, -2.f, -4.f),
			glm::vec3(-5.f,  0.f,  3.f),
			glm::vec3(2.f,  1.f, -2.f),
			glm::vec3(4.f, -3.f,  3.f),
			glm::vec3(1.f, -7.f,  1.f)
	};

	std::unique_ptr<ShaderProgram> p_shader_program;
	std::unique_ptr<ShaderProgram> p_light_source_shader_program;
	std::unique_ptr<VertexBuffer> p_cube_pos_vbo;
	std::unique_ptr<IndexBuffer> p_cube_index_buffer;
	std::unique_ptr<VertexArray> p_cube_vao;
	std::unique_ptr<Texture2D> p_texture_smile;
	std::unique_ptr<Texture2D> p_texture_quads;

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

		const unsigned int w = 1000;
		const unsigned int h = 1000;
		const unsigned int channels = 3; // rgb 
		auto* data = new unsigned char[w * h * channels];
		generate_smile_texture(data, w, h);
		p_texture_smile = std::make_unique<Texture2D>(data, w, h);
		p_texture_smile->bind(0);

		generate_quads_texture(data, w, h);
		p_texture_quads = std::make_unique<Texture2D>(data, w, h);
		p_texture_quads->bind(1);

		delete[] data;

		// First we create shader programms
		p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, frag_shader);
		if (!p_shader_program->is_compiled())
			return false;

		BufferLayout buffer_layout_vec3_vec3_vec2
		{
			ShaderDataType::Float3,
			ShaderDataType::Float3,
			ShaderDataType::Float2
		};

		// NOW we have to PASS our CPU data in shaders 
		// using VERTEX BUFFER OBJECT to allocate and fill memory on gpu
		p_cube_vao = std::make_unique<VertexArray>();
		p_cube_pos_vbo = std::make_unique<VertexBuffer>(pos_norm_uv, sizeof(pos_norm_uv), buffer_layout_vec3_vec3_vec2);
		p_cube_index_buffer = std::make_unique<IndexBuffer>(indices, sizeof(indices) / sizeof(GLuint));

		// NEXT BIND data from buffers with our shaders
		// using VERTEX ARRAY OBJECT
		p_cube_vao->add_vertex_buffer(*p_cube_pos_vbo);
		p_cube_vao->set_index_buffer(*p_cube_index_buffer);
		//-----------------------------------------//

		p_light_source_shader_program = std::make_unique<ShaderProgram>(light_source_vertex_shader, light_source_frag_shader);
		if (!p_light_source_shader_program->is_compiled())
			return false;

		Renderer_OpenGL::enable_depth_testing();
		while (!m_bCloseWindow) {
			draw();
		}

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

		p_shader_program->bind();

		p_shader_program->set_vec3("light_color",
			glm::vec3(light_source_color[0], light_source_color[1], light_source_color[2])
		);
		p_shader_program->set_vec3("light_pos_eye",
			glm::vec3(
				camera.get_view_matrix() *
				glm::vec4(light_source_pos[0], light_source_pos[1], light_source_pos[2], 1.f)
			)
		);

		p_shader_program->set_float("ambient_factor", ambient_factor);
		p_shader_program->set_float("diffuse_factor", diffuse_factor);
		p_shader_program->set_float("specular_factor", specular_factor);
		p_shader_program->set_float("shininess", shininess);

		// draw cubes
		for (const glm::vec3& current_pos : positions) {
			glm::mat4 translate_mat(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				current_pos[0], current_pos[1], current_pos[2], 1);
			const glm::mat4 model_view_mat = camera.get_view_matrix() * translate_mat;
			p_shader_program->set_matrix4("model_view_mat", model_view_mat);
			p_shader_program->set_matrix4("mvp_mat", camera.get_projection_matrix() * model_view_mat);
			p_shader_program->set_matrix3("normal_matrix",
				glm::transpose(glm::inverse(model_view_mat))
			);
			Renderer_OpenGL::draw(*p_cube_vao);
		}

		// set light source pos and color
		{
			p_light_source_shader_program->bind();
			glm::mat4 translate_mat(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				light_source_pos[0], light_source_pos[1], light_source_pos[2], 1);
			p_light_source_shader_program->set_matrix4("mvp_mat", camera.get_projection_matrix() * camera.get_view_matrix() * translate_mat);
			p_light_source_shader_program->set_vec3("light_color", glm::vec3(light_source_color[0], light_source_color[1], light_source_color[2]));
			Renderer_OpenGL::draw(*p_cube_vao);
		}

		UIModule::on_ui_draw_begin();
		on_ui_draw();
		UIModule::on_ui_draw_end();

		m_pWindow->on_update();
		on_update();
	}

}