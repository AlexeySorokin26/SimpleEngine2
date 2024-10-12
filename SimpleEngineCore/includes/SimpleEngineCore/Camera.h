#pragma once

#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace SimpleEngine {

	// We need view (camera) matrix to go from world space to camera space
	// We simulate it by inverting own model matrix of camera
	// Such matrix describes position and rotation of matrix
	// And can be achieved using 3 vectors or using Euler angles or Quternions
	// So to go from Wold Space to Camera space we invert our camera model matrix

	// Next we also need to project 
	class Camera
	{
	public:
		enum class ProjectionMode
		{
			Perspective,
			Orthographic
		};
		Camera(const glm::vec3& position = { 0, 0, 0 },
			const glm::vec3& rotation = { 0, 0, 0 },
			const ProjectionMode projection_mode = ProjectionMode::Perspective);
		void set_position(const glm::vec3& position);
		void set_rotation(const glm::vec3& rotation);
		void set_position_rotation(const glm::vec3& position, const glm::vec3& rotation);
		void set_projection_mode(const ProjectionMode projection_mode);
		glm::mat4 get_view_matrix();
		glm::mat4 get_projection_matrix() const { return m_projection_matrix; }
		// we gonna move in direction of such vectors not just by xyz
		void move_forward(const float delta);
		void move_right(const float delta);
		void move_up(const float delta);

		const glm::vec3& get_camera_pos() const { return m_position; }
		const glm::vec3& get_camera_rotation() const { return m_rotation; }

		// movement_delta.x - forward, movement_delta.y - right, movement_delta.z - up
		// rotation_delta.x - roll, rotation_delta.y - pitch, rotation_delta.z - yaw
		void add_movement_and_rotation(const glm::vec3& movement_delta, const glm::vec3& rotation_delta);
	private:
		void update_view_matrix();
		void update_projection_matrix();
		glm::vec3 m_position;
		glm::vec3 m_rotation; // X - ROLL, Y - PITCH, Z - YAW
		ProjectionMode m_projection_mode;

		glm::vec3 m_direction;
		glm::vec3 m_right;
		glm::vec3 m_up;

		static constexpr glm::vec3 s_world_up{ 0.f, 0.f, 1.f };
		static constexpr glm::vec3 s_world_right{ 0.f, -1.f, 0.f };
		static constexpr glm::vec3 s_world_forward{ 1.f, 0.f, 0.f };

		glm::mat4 m_view_matrix;
		glm::mat4 m_projection_matrix;

		bool m_update_view_matrix = false;
	};
}