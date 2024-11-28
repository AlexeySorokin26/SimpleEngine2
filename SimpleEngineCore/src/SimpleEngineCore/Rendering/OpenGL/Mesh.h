#pragma once 

#include <memory>
#include <stdexcept>
#include <array>
#include <filesystem>
#include <cmath>
#include <cstring>  // for memcpy

#include "SimpleEngineCore/Rendering/OpenGL/VertexBuffer.h"
#include "SimpleEngineCore/Rendering/OpenGL/VertexArray.h"
#include "SimpleEngineCore/Rendering/OpenGL/IndexBuffer.h"
#include "SimpleEngineCore/Rendering/OpenGL/ShaderProgram.h"
#include "SimpleEngineCore/Rendering/OpenGL/Texture2D.h"
#include "SimpleEngineCore/Rendering/OpenGL/Material.h"
#include "SimpleEngineCore/Rendering/OpenGL/Light.h"
#include "SimpleEngineCore/Rendering/OpenGL/Renderer_OpenGL.h"
#include "SimpleEngineCore/Camera.h"
#include "SimpleEngineCore/Utils.h"

#include <GLFW/glfw3.h>

#include <glm/mat3x3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>


namespace SimpleEngine {

	class ShaderCompilationException : public std::runtime_error {
	public:
		ShaderCompilationException(const std::string& message)
			: std::runtime_error(message) {}
	};

	class LightCube {
	public:
		LightCube(const std::vector<GLfloat> vertices, const std::vector<GLuint> indices)
		{
			std::filesystem::path shaderPath = getBasePath() / "shaders";
			std::filesystem::path vertex_shader_name = shaderPath / "light_cube_vertex_shader.glsl";
			std::filesystem::path frag_shader_name = shaderPath / "light_cube_fragment_shader.glsl";
			p_shader_program = std::make_unique<ShaderProgram>(
				vertex_shader_name.string(), frag_shader_name.string());
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

			// VBO
			if (sizeof(vertices) > 0) {
				p_vbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(GLfloat), buffer_layout_vec3_vec3_vec2);
				p_vao->add_vertex_buffer(*p_vbo);
			}
			// INDEX BUFFER
			if (sizeof(indices) > 0) {
				p_index_buffer = std::make_unique<IndexBuffer>(indices.data(), indices.size());
				p_vao->set_index_buffer(*p_index_buffer);
			}
		}
		void draw(Camera& camera, const PointLight& light) {
			p_shader_program->bind();

			// draw light cube
			{
				glm::mat4 translate_mat(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					light.position[0], light.position[1], light.position[2], 1);
				p_shader_program->set_matrix4("mvp_mat",
					camera.get_projection_matrix() * camera.get_updated_view_matrix() * translate_mat);

				light.UseLight(
					p_shader_program->get_uniform_location("light_ambient"),
					p_shader_program->get_uniform_location("light_diffuse"),
					p_shader_program->get_uniform_location("light_specular"), // TODO rest should be deleted 
					p_shader_program->get_uniform_location("pointLight.position"),
					p_shader_program->get_uniform_location("pointLight.ambientIntensity"),
					p_shader_program->get_uniform_location("pointLight.diffuseIntensity"),
					p_shader_program->get_uniform_location("pointLight.specularIntensity"),
					p_shader_program->get_uniform_location("pointLight.constant"),
					p_shader_program->get_uniform_location("pointLight.linear"),
					p_shader_program->get_uniform_location("pointLight.quadratic"));
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
		Cube(const Material& material,
			glm::vec3 position = glm::vec3{ -2.f, -2.f, 40.f },
			const std::string& texturePath = "",
			const std::string& texturePath1 = "",
			const std::vector<GLfloat>& vertices = {},
			const std::vector<GLuint>& indices = {})
			: material(material), position(position), vertices(vertices), indices(indices)
		{
			std::filesystem::path shaderPath = getBasePath() / "shaders";
			std::filesystem::path vertex_shader_name = shaderPath / "phong_cube_vertex_shader.glsl";
			std::filesystem::path frag_shader_name = shaderPath / "phong_cube_fragment_shader.glsl";
			p_shader_program = std::make_unique<ShaderProgram>(
				vertex_shader_name.string(), frag_shader_name.string());

			if (!p_shader_program->is_compiled())
				throw ShaderCompilationException("Shader compilation failed");

			// VAO
			p_vao = std::make_unique<VertexArray>();
			p_vao->bind();

			BufferLayout buffer_layout_vec3_vec3_vec2{
				ShaderDataType::Float3,
				ShaderDataType::Float3,
				ShaderDataType::Float2
			};

			// VBO
			if (!vertices.empty()) {
				p_vbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(GLfloat), buffer_layout_vec3_vec3_vec2);
				p_vao->add_vertex_buffer(*p_vbo);
			}

			// INDEX BUFFER
			if (!indices.empty()) {
				p_index_buffer = std::make_unique<IndexBuffer>(indices.data(), indices.size());
				p_vao->set_index_buffer(*p_index_buffer);
			}

			if (!texturePath.empty()) {
				const unsigned int w = 1000;
				const unsigned int h = 1000;
				p_texture = std::make_unique<Texture2D>(texturePath, w, h);
			}

			if (!texturePath1.empty()) {
				const unsigned int w = 1000;
				const unsigned int h = 1000;
				p_texture1 = std::make_unique<Texture2D>(texturePath1, w, h);
			}
		}

		void draw(Camera& camera, const DirectionalLight& dirLight, bool useDirLight, const PointLight& pointLight, bool usePointLight, bool useSpotLight, const glm::vec3 scale_factor)
		{
			p_shader_program->bind();

			// Textures
			if (p_texture) {
				p_shader_program->set_int("material.diffuse", 0);
				p_texture->bind(0);
			}

			if (p_texture1) {
				p_shader_program->set_int("material.specular", 1);
				p_texture1->bind(1);
			}

			p_shader_program->set_vec3("globalAmbient", glm::vec3{ 0.2,0.2,0.2 });

			dirLight.UseLight(
				p_shader_program->get_uniform_location("directionalLight.ambient"),
				p_shader_program->get_uniform_location("directionalLight.diffuse"),
				p_shader_program->get_uniform_location("directionalLight.specular"),
				p_shader_program->get_uniform_location("directionalLight.direction"),
				p_shader_program->get_uniform_location("directionalLight.ambientIntensity"),
				p_shader_program->get_uniform_location("directionalLight.diffuseIntensity"),
				p_shader_program->get_uniform_location("directionalLight.specularIntensity")
			);

			pointLight.UseLight(
				p_shader_program->get_uniform_location("pointLight.ambient"),
				p_shader_program->get_uniform_location("pointLight.diffuse"),
				p_shader_program->get_uniform_location("pointLight.specular"),
				p_shader_program->get_uniform_location("pointLight.position"),
				p_shader_program->get_uniform_location("pointLight.ambientIntensity"),
				p_shader_program->get_uniform_location("pointLight.diffuseIntensity"),
				p_shader_program->get_uniform_location("pointLight.specularIntensity"),
				p_shader_program->get_uniform_location("pointLight.constant"),
				p_shader_program->get_uniform_location("pointLight.linear"),
				p_shader_program->get_uniform_location("pointLight.quadratic")
			);

			// Cam 
			p_shader_program->set_vec3("cam_pos", camera.get_camera_pos());

			// Use light
			p_shader_program->set_int("useDirLight", useDirLight);
			p_shader_program->set_int("usePointLight", usePointLight);
			p_shader_program->set_int("useSpotLight", useSpotLight);

			// material
			material.UseMaterial(
				p_shader_program->get_uniform_location("material.ambient"),
				p_shader_program->get_uniform_location("material.shininess"));

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
		std::unique_ptr<Texture2D> p_texture1;

		glm::vec3 position;
		std::vector<GLfloat> vertices;
		std::vector<GLuint> indices;
	};


}