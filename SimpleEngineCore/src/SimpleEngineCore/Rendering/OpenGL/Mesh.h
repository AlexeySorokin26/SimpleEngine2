#pragma once 

#include <memory>
#include <stdexcept>
#include <array>

#include "SimpleEngineCore/Rendering/OpenGL/VertexBuffer.h"
#include "SimpleEngineCore/Rendering/OpenGL/VertexArray.h"
#include "SimpleEngineCore/Rendering/OpenGL/IndexBuffer.h"
#include "SimpleEngineCore/Rendering/OpenGL/ShaderProgram.h"
#include "SimpleEngineCore/Rendering/OpenGL/Texture2D.h"
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

	struct Material {
		float ambient_factor = { 0.1f };
		float diffuse_factor = { 1.0f };
		float specular_factor = { 0.5f };
		float shininess = { 32.f };
	};

	class ShaderCompilationException : public std::runtime_error {
	public:
		ShaderCompilationException(const std::string& message)
			: std::runtime_error(message) {}
	};

	class Cube {
	public:
		Cube(const Material& material)
			: material(material)
		{
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

			p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, frag_shader);
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
		}

		void draw(Camera& camera, const float light_source_color[3], const float light_source_pos[3], const float scale_factor) {
			p_shader_program->bind();

			p_shader_program->set_vec3("light_color",
				glm::vec3(light_source_color[0], light_source_color[1], light_source_color[2])
			);
			p_shader_program->set_vec3("light_pos_eye",
				glm::vec3(
					camera.get_updated_view_matrix() *
					glm::vec4(light_source_pos[0], light_source_pos[1], light_source_pos[2], 1.f)
				)
			);

			p_shader_program->set_float("ambient_factor", material.ambient_factor);
			p_shader_program->set_float("diffuse_factor", material.diffuse_factor);
			p_shader_program->set_float("specular_factor", material.specular_factor);
			p_shader_program->set_float("shininess", material.shininess);

			// draw cubes
			{
				glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), glm::vec3(scale_factor));
				for (const glm::vec3& current_pos : positions) {
					glm::mat4 translate_mat(
						1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						current_pos[0], current_pos[1], current_pos[2], 1);

					const glm::mat4 model_view_mat = camera.get_updated_view_matrix() * translate_mat * scale_mat;
					p_shader_program->set_matrix4("model_view_mat", model_view_mat);
					p_shader_program->set_matrix4("mvp_mat", camera.get_projection_matrix() * model_view_mat);
					p_shader_program->set_matrix3("normal_matrix",
						glm::transpose(glm::inverse(model_view_mat))
					);

					Renderer_OpenGL::draw(*p_vao);
				}
			}
		}
	private:
		Material material;
		std::unique_ptr<ShaderProgram> p_shader_program;
		std::unique_ptr<VertexArray> p_vao;
		std::unique_ptr<VertexBuffer> p_vbo;
		std::unique_ptr<IndexBuffer> p_index_buffer;
		std::unique_ptr<Texture2D> p_texture_smile;
		std::unique_ptr<Texture2D> p_texture_quads;

		std::array<glm::vec3, 1> positions = {
		glm::vec3(0.f, 0.f, 0.f) };
		/*std::array<glm::vec3, 5> positions = {
		glm::vec3(-2.f, -2.f, -4.f),
		glm::vec3(-5.f,  0.f,  3.f),
		glm::vec3(2.f,  1.f, -2.f),
		glm::vec3(4.f, -3.f,  3.f),
		glm::vec3(1.f, -7.f,  1.f)
		};*/
	};


	class LightCube {
	public:
		LightCube()
		{
			const char* vertex_shader =
				R"(
			#version 460

			layout(location=0) in vec3 vertex_position; 
			layout(location=1) in vec3 vertex_normal; 
			layout(location=2) in vec2 texture_coord;

			uniform mat4 mvp_mat;

			void main() {
				gl_Position = mvp_mat * vec4(vertex_position * 0.05f, 1.0); 
			})";

			const char* frag_shader =
				R"(
			#version 460		
			out vec4 frag_color;

			uniform vec3 light_color;

			void main() {
					frag_color = vec4(light_color, 1.0);
			})";

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

			p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, frag_shader);
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

	// More complicated than I thought. Could use from GrIT solution but hard to add for now.
	//class WorldCoordinateSystem {
	//public:
	//	WorldCoordinateSystem()
	//	{
	//		const char* vertex_shader =
	//			R"(
	//		#version 460

	//		layout(location=0) in vec3 vertex_position; 
	//		layout(location = 1) in vec3 color;

	//		uniform mat4 mvp_mat;
	//		out vec3 frag_color;

	//		void main() {
	//			gl_Position = mvp_mat * vec4(vertex_position, 1.0);
	//			frag_color = color;
	//		})";

	//		const char* frag_shader =
	//			R"(
	//		#version 460		
	//		in vec3 frag_color;
	//		out vec4 color;

	//		void main() {
	//				color = vec4(frag_color, 1.0);
	//		})";

	//		GLfloat vertices[] = {
	//			// X-axis (Red)
	//			// (x,y,z) -> (r,g,b)
	//			0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // Origin
	//			1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // X-axis

	//			// Y-axis (Green)
	//			0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // Origin
	//			0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // Y-axis

	//			// Z-axis (Blue)
	//			0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // Origin
	//			0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f   // Z-axis
	//		};

	//		GLuint indices[] = {
	//			0,	1,  2,  3,  4,  5
	//		};

	//		p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, frag_shader);
	//		if (!p_shader_program->is_compiled())
	//			throw ShaderCompilationException("Shader compilation failed");
	//		// VAO
	//		p_vao = std::make_unique<VertexArray>();
	//		p_vao->bind();

	//		BufferLayout buffer_layout_vec3_vec3
	//		{
	//			ShaderDataType::Float3,
	//			ShaderDataType::Float3
	//		};

	//		// todo might find better way to store data. vector?
	//		// VBO
	//		if (sizeof(vertices) > 0) {
	//			p_vbo = std::make_unique<VertexBuffer>(vertices, sizeof(vertices), buffer_layout_vec3_vec3);
	//			p_vao->add_vertex_buffer(*p_vbo);
	//		}
	//		// INDEX BUFFER
	//		if (sizeof(indices) > 0) {
	//			p_index_buffer = std::make_unique<IndexBuffer>(indices, sizeof(indices) / sizeof(GLuint));
	//			p_vao->set_index_buffer(*p_index_buffer);
	//		}
	//	}

	//	void draw(Camera& camera, float windowWidth, float windowHeight) {
	//		// Bind the shader program
	//		p_shader_program->bind();

	//		// Disable depth testing so the axes are drawn on top
	//		Renderer_OpenGL::disable_depth_testing();

	//		// Retrieve the camera's position and orientation
	//		glm::vec3 camera_position = camera.get_position();
	//		glm::mat4 camera_rotation = glm::mat4(glm::mat3(camera.get_updated_view_matrix()));

	//		// Define the offset from the camera's position where the coordinate system will appear
	//		glm::vec3 offset_from_camera(0.8f, 0.8f, -1.0f); // Adjust as needed to position it in the top-right
	//		glm::vec3 target_position = camera_position + offset_from_camera;

	//		// Create a translation matrix to position the coordinate system
	//		glm::mat4 model = glm::translate(glm::mat4(1.0f), target_position);

	//		// Apply a fixed scale for consistent size
	//		float fixed_scale = 0.05f; // Adjust for desired size
	//		model = glm::scale(model, glm::vec3(fixed_scale));

	//		// Set up the MVP matrix: apply the camera's rotation, then the translation and scale
	//		glm::mat4 mvp = camera.get_projection_matrix() * camera.get_updated_view_matrix() * model * camera_rotation;

	//		// Pass the MVP matrix to the shader
	//		p_shader_program->set_matrix4("mvp_mat", mvp);

	//		// Draw the coordinate axes
	//		Renderer_OpenGL::draw_arrays(*p_vao);

	//		// Re-enable depth testing
	//		Renderer_OpenGL::enable_depth_testing();
	//	}


	//private:
	//	std::unique_ptr<ShaderProgram> p_shader_program;
	//	std::unique_ptr<VertexArray> p_vao;
	//	std::unique_ptr<VertexBuffer> p_vbo;
	//	std::unique_ptr<IndexBuffer> p_index_buffer;
	//};
}