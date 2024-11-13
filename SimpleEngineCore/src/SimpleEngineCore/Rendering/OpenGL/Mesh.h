#pragma once 

#include <memory>
#include <stdexcept>
#include <array>

#include "SimpleEngineCore/Rendering/OpenGL/VertexBuffer.h"
#include "SimpleEngineCore/Rendering/OpenGL/VertexArray.h"
#include "SimpleEngineCore/Rendering/OpenGL/IndexBuffer.h"
#include "SimpleEngineCore/Rendering/OpenGL/ShaderProgram.h"
#include "SimpleEngineCore/Rendering/OpenGL/Texture2D.h"
#include "SimpleEngineCore/Rendering/OpenGL/Material.h"
#include "SimpleEngineCore/Rendering/OpenGL/Renderer_OpenGL.h"
#include "SimpleEngineCore/Camera.h"

#include <GLFW/glfw3.h>

#include <glm/mat3x3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace SimpleEngine {

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
		generate_circle(data, width, height,
			static_cast<unsigned int>(width * 0.5),
			static_cast<unsigned int>(height * 0.5),
			static_cast<unsigned int>(width * 0.4), 255, 255, 0);

		// smile
		generate_circle(data, width, height,
			static_cast<unsigned int>(width * 0.5),
			static_cast<unsigned int>(height * 0.4),
			static_cast<unsigned int>(width * 0.2), 0, 0, 0);

		generate_circle(data, width, height,
			static_cast<unsigned int>(width * 0.5),
			static_cast<unsigned int>(height * 0.45),
			static_cast<unsigned int>(width * 0.2), 255, 255, 0);

		// eyes
		generate_circle(data, width, height,
			static_cast<unsigned int>(width * 0.35),
			static_cast<unsigned int>(height * 0.6),
			static_cast<unsigned int>(width * 0.07), 255, 0, 255);

		generate_circle(data, width, height,
			static_cast<unsigned int>(width * 0.65),
			static_cast<unsigned int>(height * 0.6),
			static_cast<unsigned int>(width * 0.07), 0, 0, 255);
	}



	class ShaderCompilationException : public std::runtime_error {
	public:
		ShaderCompilationException(const std::string& message)
			: std::runtime_error(message) {}
	};

	class LightCube {
	public:
		LightCube()
		{
			GLfloat vertices[] = {
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

			std::string base_path = "C:\\Users\\sorok\\Desktop\\SimpleEngine2\\SimpleEngineCore\\shaders\\";
			std::string vertex_shader_name = "light_cube_vertex_shader.glsl";
			std::string frag_shader_name = "light_cube_fragment_shader.glsl";
			p_shader_program = std::make_unique<ShaderProgram>(
				base_path + vertex_shader_name, base_path + frag_shader_name);
			if (!p_shader_program->is_compiled())
				throw ShaderCompilationException("Shader compilation failed");
			// VAO
			p_vao = std::make_unique<VertexArray>();
			p_vao->bind();

			// todo it works only for cube for now
			BufferLayout buffer_layout_vec3_vec3_vec2
			{
				ShaderDataType::Float3,
				ShaderDataType::Float3,
				ShaderDataType::Float2
			};

			// todo might find better way to store data. vector?
			// VBO
			if (sizeof(vertices) > 0) {
				p_vbo = std::make_unique<VertexBuffer>(vertices, sizeof(vertices), buffer_layout_vec3_vec3_vec2);
				p_vao->add_vertex_buffer(*p_vbo);
			}
			// INDEX BUFFER
			if (sizeof(indices) > 0) {
				p_index_buffer = std::make_unique<IndexBuffer>(indices, sizeof(indices) / sizeof(GLuint));
				p_vao->set_index_buffer(*p_index_buffer);
			}
		}

		void draw(Camera& camera, const float light_source_color[3], const float light_source_pos[3]) {
			p_shader_program->bind();

			// draw light cube
			{
				glm::mat4 translate_mat(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					light_source_pos[0], light_source_pos[1], light_source_pos[2], 1);
				p_shader_program->set_matrix4("mvp_mat",
					camera.get_projection_matrix() * camera.get_updated_view_matrix() * translate_mat);
				p_shader_program->set_vec3("light_color",
					glm::vec3(light_source_color[0], light_source_color[1], light_source_color[2]));
				Renderer_OpenGL::draw(*p_vao);
			}
		}
	private:
		std::unique_ptr<ShaderProgram> p_shader_program;
		std::unique_ptr<VertexArray> p_vao;
		std::unique_ptr<VertexBuffer> p_vbo;
		std::unique_ptr<IndexBuffer> p_index_buffer;
	};

	class Cube {
	public:
		Cube(const Material& material, int unit = 0, glm::vec3 position = glm::vec3{ -2.f, -2.f, 4.f }, const std::string& texturePath = "")
			: material(material), unit(unit), position(position)
		{
			GLfloat vertices[] = {
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

			std::string base_path = "C:\\Users\\sorok\\Desktop\\SimpleEngine2\\SimpleEngineCore\\shaders\\";
			std::string vertex_shader_name = "phong_cube_vertex_shader.glsl";
			std::string frag_shader_name = "phong_cube_fragment_shader.glsl";
			p_shader_program = std::make_unique<ShaderProgram>(
				base_path + vertex_shader_name, base_path + frag_shader_name);
			if (!p_shader_program->is_compiled())
				throw ShaderCompilationException("Shader compilation failed");
			// VAO
			p_vao = std::make_unique<VertexArray>();
			p_vao->bind();

			BufferLayout buffer_layout_vec3_vec3_vec2
			{
				ShaderDataType::Float3,
				ShaderDataType::Float3,
				ShaderDataType::Float2
			};

			// VBO
			if (sizeof(vertices) > 0) {
				p_vbo = std::make_unique<VertexBuffer>(vertices, sizeof(vertices), buffer_layout_vec3_vec3_vec2);
				p_vao->add_vertex_buffer(*p_vbo);
			}
			// INDEX BUFFER
			if (sizeof(indices) > 0) {
				p_index_buffer = std::make_unique<IndexBuffer>(indices, sizeof(indices) / sizeof(GLuint));
				p_vao->set_index_buffer(*p_index_buffer);
			}

			if (!texturePath.empty()) {
				const unsigned int w = 1000;
				const unsigned int h = 1000;
				p_texture = std::make_unique<Texture2D>(texturePath, w, h);
				p_texture->bind();
			}
		}

		void draw(Camera& camera,
			const float light_source_color[3], const float light_source_pos[3], const glm::vec3 scale_factor)
		{
			p_shader_program->bind();
			p_texture->bind();

			p_shader_program->set_vec3("cube_color",
				glm::vec3(material.color[0], material.color[1], material.color[2]));

			p_shader_program->set_vec3("light_color",
				glm::vec3(light_source_color[0], light_source_color[1], light_source_color[2]));

			p_shader_program->set_vec3("light_pos",
				glm::vec3(light_source_pos[0], light_source_pos[1], light_source_pos[2]));

			p_shader_program->set_vec3("cam_pos",
				camera.get_camera_pos());

			// todo combine into material
			p_shader_program->set_float("ambient_factor",
				material.ambient_factor);

			p_shader_program->set_float("diffuse_factor",
				material.diffuse_factor);

			p_shader_program->set_float("specular_factor",
				material.specular_factor);

			p_shader_program->set_float("shininess",
				material.shininess);

			// draw cubes
			{
				glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), scale_factor);
				glm::mat4 translate_mat(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					position[0], position[1], position[2], 1);

				const glm::mat4 m_mat =
					translate_mat * scale_mat;
				p_shader_program->set_matrix4("m_mat", m_mat);

				const glm::mat4 normal_mat =
					glm::mat3(transpose(inverse(m_mat)));
				p_shader_program->set_matrix3("normal_mat", normal_mat);

				const glm::mat4 mvp_mat =
					camera.get_projection_matrix() * camera.get_updated_view_matrix() * m_mat;
				p_shader_program->set_matrix4("mvp_mat", mvp_mat);

				Renderer_OpenGL::draw(*p_vao);
			}
		}

	private:
		Material material;
		std::unique_ptr<ShaderProgram> p_shader_program;
		std::unique_ptr<VertexArray> p_vao;
		std::unique_ptr<VertexBuffer> p_vbo;
		std::unique_ptr<IndexBuffer> p_index_buffer;
		std::unique_ptr<Texture2D> p_texture;

		glm::vec3 position;
		int unit = 0;
	};
}