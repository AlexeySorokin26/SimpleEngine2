#pragma once

#include "SimpleEngineCore/Event.h"
#include "SimpleEngineCore/Camera.h"

#include <memory>

namespace SimpleEngine {

	class Application {
	public:
		Application();
		virtual ~Application();

		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		virtual int start(unsigned int window_width, unsigned int window_heigth, const char* title);
		void close() { m_bCloseWindow = true; }
		virtual void on_update() {};

		virtual void on_ui_draw() {};

		virtual void on_mouse_button_event(const MouseButtonCode button_code, const double x_pos, const double y_pos, const bool pressed) {};
		glm::vec2 get_current_cursor_pos() const;

		Camera camera{ glm::vec3(-5, 0, 0) };

		float m_background_color[4] = { 0, 0, 0, 1 };

		glm::vec3 directional_light_direction{ 0.f, 0.f, 1.f };
		glm::vec3 point_light_position{ 0.f, 0.f, 1.f };
		glm::vec3 light_ambient_factor{ 1.f, 1.f, 1.1f };
		float light_ambient_intensity{ 0.1f };
		glm::vec3 light_diffuse_factor{ 1.f, 1.f, 1.1f };
		float light_diffuse_intensity{ 0.4f };
		glm::vec3 light_specular_factor{ 1.f, 1.f, 1.1f };
		float light_specular_intensity{ 0.5f };

		float shininess = { 32.f };
		float scale_factor = { 1.f };

		float cube_shininess = { 32.f };
		float cube_scale_factor = { 1.f };

		bool scroll = false;
		bool scrollUp = false;
	private:
		void draw();
		std::unique_ptr<class Window> m_pWindow;

		EventDispatcher m_event_dispatcher;
		bool m_bCloseWindow = false;
	};
}