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
		float light_source_pos[3] = { 0.f, 0.f, 1.f };
		float light_source_color[3] = { 1.f, 1.f, 1.f };
		float ambient_factor = { 0.1f };
		float diffuse_factor = { 1.0f };
		float specular_factor = { 0.5f };
		float shininess = { 32.f };

		bool scroll = false;
		bool scrollUp = false;
	private:
		void draw();
		std::unique_ptr<class Window> m_pWindow;

		EventDispatcher m_event_dispatcher;
		bool m_bCloseWindow = false;
	};
}