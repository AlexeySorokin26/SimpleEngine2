#include <iostream>
#include <memory>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "SimpleEngineCore/Application.h"
#include <SimpleEngineCore/Input.h>

#include <filesystem>

#include <glm/gtc/type_ptr.hpp>

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

		if (SimpleEngine::Input::IsKeyPressed(SimpleEngine::KeyCode::KEY_LEFT_CONTROL)) {
			if (scroll) {
				if (scrollUp)
					movment_delta.x += 0.1f;
				else
					movment_delta.x -= 0.1f;
				scroll = false;
			}
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
			// if only right then just rotate in z and y
			else {
				// mouse have only x and y pos from screen but we want to rotate in z and y
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

	void setup_dockspace_menu()
	{
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoWindowMenuButton;
		static ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		window_flags |= ImGuiWindowFlags_NoBackground;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(3);
		ImGuiIO& io = ImGui::GetIO();
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene...", NULL))
				{
				}
				if (ImGui::MenuItem("Open Scene...", NULL))
				{
				}
				if (ImGui::MenuItem("Save Scene...", NULL))
				{
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit", NULL))
				{
					close();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}

	virtual void on_ui_draw() override {
		setup_dockspace_menu();

		camera_pos[0] = camera.get_camera_pos().x;
		camera_pos[1] = camera.get_camera_pos().y;
		camera_pos[2] = camera.get_camera_pos().z;

		camera_rotation[0] = camera.get_camera_rotation().x;
		camera_rotation[1] = camera.get_camera_rotation().y;
		camera_rotation[2] = camera.get_camera_rotation().z;

		camera_fov = camera.get_fov();
		camera_far_plane = camera.get_far_clip_plane();
		camera_near_plane = camera.get_near_clip_plane();

		ImGui::Begin("Editor");
		// if we change value
		ImGui::Checkbox("Use point light", &usePointLight);
		ImGui::Checkbox("Use directional light", &useDirectionalLight);
		ImGui::Checkbox("Use spotlight", &useSpotLight);
		ImGui::SliderFloat3("Point light position", glm::value_ptr(point_light_position), -10.f, 10.f);
		ImGui::SliderFloat3("Directional light direction", glm::value_ptr(directional_light_direction), -1.f, 1.f);
		ImGui::SliderFloat3("Light Ambient factor", glm::value_ptr(light_ambient_factor), 0.1f, 1.f);
		ImGui::SliderFloat("Light Ambient intensity", &light_ambient_intensity, 0.1f, 1.f);
		ImGui::SliderFloat3("Light Diffuse factor", glm::value_ptr(light_diffuse_factor), 0.1f, 1.f);
		ImGui::SliderFloat("Light Diffuse intensity", &light_diffuse_intensity, 0.1f, 1.f);
		ImGui::SliderFloat3("Light Specular factor", glm::value_ptr(light_specular_factor), 0.1f, 1.f);
		ImGui::SliderFloat("Light Specular intensity", &light_specular_intensity, 0.1f, 1.f);
		ImGui::SliderFloat("Material Shininess", &cube_shininess, 1.f, 128.f);
		ImGui::SliderFloat("Cube scale", &cube_scale_factor, 0.001f, 100.f);

		if (ImGui::SliderFloat3("Cam pos", camera_pos, -10.f, 10.f)) {
			camera.set_position(glm::vec3(camera_pos[0], camera_pos[1], camera_pos[2]));
		}
		if (ImGui::SliderFloat3("Cam rotation", camera_rotation, 0.f, 360)) {
			camera.set_rotation(glm::vec3(camera_rotation[0], camera_rotation[1], camera_rotation[2]));
		}
		if (ImGui::Checkbox("Perspective camera", &perspective_camera)) {
			camera.set_projection_mode(perspective_camera ? SimpleEngine::Camera::ProjectionMode::Perspective : SimpleEngine::Camera::ProjectionMode::Orthographic);
		}
		if (ImGui::SliderFloat("Cam fov", &camera_fov, 1.f, 120.f)) {
			camera.set_fov(camera_fov);
		}
		if (ImGui::SliderFloat("Cam near clip plane", &camera_near_plane, 0.1f, 10.f)) {
			camera.set_near_clip_plane(camera_near_plane);
		}
		if (ImGui::SliderFloat("Cam far clip plane", &camera_far_plane, 1.f, 100.f)) {
			camera.set_far_clip_plane(camera_far_plane);
		}

		ImGui::End();
	}

	double m_initial_mouse_pos_x = 0.0;
	double m_initial_mouse_pos_y = 0.0;
	int frame = 0;

	float camera_pos[3] = { 0.f, 0.f, 1.f };
	float camera_rotation[3] = { 0.f, 0.f, 0.f };
	float camera_fov = 60.f;
	float camera_near_plane = 0.1f;
	float camera_far_plane = 100.f;

	bool perspective_camera = true;
};


int main() {

	int returnCode = 0;
	try
	{
		auto myApp = std::make_unique<SimpleEngineEditor>();

		returnCode = myApp->start(2048, 2048, "My first app");
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return returnCode;
}