#include "SimpleEngineCore/Application.h"
#include "SimpleEngineCore/Window.h"
#include "SimpleEngineCore/Log.h"

#include <iostream>

namespace SimpleEngine {
	Application::Application() {
		LOG_INFO("Starting Application");
	}

	Application::~Application() {
		LOG_INFO("Closing Application");
	}

	int Application::start(unsigned int window_width, unsigned int window_heigth, const char* title) {
		m_pWindow = std::make_unique<Window>(title, window_width, window_heigth);
		m_event_dispatcher.add_event_listener<EventMouseMoved>(
			[](EventMouseMoved& event) {
				LOG_INFO("[MouseMoved] Mouse moved to {0}x{1}", event.x, event.y);
			}
		);
		m_event_dispatcher.add_event_listener<EventWindowResize>(
			[](EventWindowResize& event) {
				LOG_INFO("[WindowResized] Window resized to {0}x{1}", event.w, event.h);
			}
		);
		m_pWindow->set_event_callback(
			[&](BaseEvent& event) {
				m_event_dispatcher.dispatch(event);
			});
		while (true) {
			m_pWindow->on_update();
			on_update();
		}

		return 0;
	}

	void Application::on_update() {

	}
}