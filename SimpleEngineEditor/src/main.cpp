#include <iostream>
#include <memory>
#include <imgui/imgui.h>

#include "SimpleEngineCore/Application.h"
#include <SimpleEngineCore/Input.h>

class SimpleEngineEditor : public SimpleEngine::Application {

	virtual void on_update() override {
		glm::vec3 movment_delta{ 0,0,0 };
		glm::vec3 rotation_delta{ 0,0,0 };
		// z
		if (SimpleEngine::Input::IsKeyPressed(SimpleEngine::KeyCode::KEY_W))
		{
			movment_delta.x += 0.05f;
		}
		// z
		if (SimpleEngine::Input::IsKeyPressed(SimpleEngine::KeyCode::KEY_S))
		{
			movment_delta.x -= 0.05f;
		}
		// x
		if (SimpleEngine::Input::IsKeyPressed(SimpleEngine::KeyCode::KEY_A))
		{
			movment_delta.y -= 0.05f;
		}
		// x
		if (SimpleEngine::Input::IsKeyPressed(SimpleEngine::KeyCode::KEY_D))
		{
			movment_delta.y += 0.05f;
		}
		// y
		if (SimpleEngine::Input::IsKeyPressed(SimpleEngine::KeyCode::KEY_E))
		{
			movment_delta.z += 0.05f;
		}
		// y
		if (SimpleEngine::Input::IsKeyPressed(SimpleEngine::KeyCode::KEY_Q))
		{
			movment_delta.z -= 0.05f;
		}
		if (SimpleEngine::Input::IsKeyPressed(SimpleEngine::KeyCode::KEY_UP))
		{
			rotation_delta.y -= 0.05f;
		}
		if (SimpleEngine::Input::IsKeyPressed(SimpleEngine::KeyCode::KEY_DOWN))
		{
			rotation_delta.y += 0.05f;
		}
		if (SimpleEngine::Input::IsKeyPressed(SimpleEngine::KeyCode::KEY_RIGHT))
		{
			rotation_delta.z -= 0.05f;
		}
		if (SimpleEngine::Input::IsKeyPressed(SimpleEngine::KeyCode::KEY_LEFT))
		{
			rotation_delta.z += 0.05f;
		}
		if (SimpleEngine::Input::IsKeyPressed(SimpleEngine::KeyCode::KEY_P))
		{
			rotation_delta.x += 0.05f;
		}
		if (SimpleEngine::Input::IsKeyPressed(SimpleEngine::KeyCode::KEY_O))
		{
			rotation_delta.x -= 0.05f;
		}


		if (SimpleEngine::Input::IsMouseButtonPressed(SimpleEngine::MouseButtonCode::MOUSE_BUTTON_RIGHT))
		{
			glm::vec2 current_cursor_pos = get_current_cursor_pos();

			// if right and left then we move up and right 
			if (SimpleEngine::Input::IsMouseButtonPressed(SimpleEngine::MouseButtonCode::MOUSE_BUTTON_LEFT))
			{
				camera.move_right(static_cast<float>(current_cursor_pos.x - m_initial_mouse_pos_x) / 100.f);
				camera.move_up((static_cast<float>(m_initial_mouse_pos_y) - current_cursor_pos.y) / 100.f);
			}
			// if only right then just rotate in x and y
			else {
				rotation_delta.z += (static_cast<float>(m_initial_mouse_pos_x) - current_cursor_pos.x) / 5.f; // to not to fast devide by 5
				rotation_delta.y -= (static_cast<float>(m_initial_mouse_pos_y) - current_cursor_pos.y) / 5.f; // to not to fast devide by 5

			}
			// update 
			m_initial_mouse_pos_x = current_cursor_pos.x;
			m_initial_mouse_pos_y = current_cursor_pos.y;
		}

		camera.add_movement_and_rotation(movment_delta, rotation_delta);
	}

	virtual void on_mouse_button_event(const SimpleEngine::MouseButtonCode button_code, const double x_pos, const double y_pos, const bool pressed) override
	{
		m_initial_mouse_pos_x = x_pos;
		m_initial_mouse_pos_y = y_pos;

	};


	virtual void on_ui_draw() override {
		camera_pos[0] = camera.get_camera_pos().x;
		camera_pos[1] = camera.get_camera_pos().y;
		camera_pos[2] = camera.get_camera_pos().z;

		camera_rotation[0] = camera.get_camera_rotation().x;
		camera_rotation[1] = camera.get_camera_rotation().y;
		camera_rotation[2] = camera.get_camera_rotation().z;

		ImGui::Begin("Editor");
		// if we change value
		if (ImGui::SliderFloat3("Cam pos", camera_pos, -10.f, 10.f)) {
			camera.set_position(glm::vec3(camera_pos[0], camera_pos[1], camera_pos[2]));
		}
		if (ImGui::SliderFloat3("Cam rotation", camera_rotation, 0.f, 360)) {
			camera.set_rotation(glm::vec3(camera_rotation[0], camera_rotation[1], camera_rotation[2]));
		}
		if (ImGui::Checkbox("Perspective camera", &perspective_camera)) {
			camera.set_projection_mode(perspective_camera ? SimpleEngine::Camera::ProjectionMode::Perspective : SimpleEngine::Camera::ProjectionMode::Orthographic);
		}
		ImGui::End();
	}

	double m_initial_mouse_pos_x = 0.0;
	double m_initial_mouse_pos_y = 0.0;
	int frame = 0;
};


int main() {
	auto myApp = std::make_unique<SimpleEngineEditor>();

	int returnCode = myApp->start(1024, 1024, "My first app");

	return returnCode;
}