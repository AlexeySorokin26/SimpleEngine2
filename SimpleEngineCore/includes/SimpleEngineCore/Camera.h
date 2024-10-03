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
        glm::mat4 get_view_matrix() const { return m_view_matrix; }
        glm::mat4 get_projection_matrix() const { return m_projection_matrix; }
    private:
        void update_view_matrix();
        void update_projection_matrix();
        glm::vec3 m_position;
        glm::vec3 m_rotation;
        ProjectionMode m_projection_mode;
        glm::mat4 m_view_matrix;
        glm::mat4 m_projection_matrix;
    };
}