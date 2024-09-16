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

		while (true) {
			m_pWindow->on_update();
			on_update();
		}

		return 0;
	}

	void Application::on_update() {

	}
}