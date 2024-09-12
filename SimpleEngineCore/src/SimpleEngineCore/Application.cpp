#include "SimpleEngineCore/Application.h"
#include "SimpleEngineCore/Log.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace SimpleEngine {
	Application::Application() {
		LOG_INFO("Welcome");
	}

	Application::~Application() {

	}

	int Application::start(unsigned int window_width, unsigned int window_heith, const char* title) {
		GLFWwindow* window;

		/* Initialize the library */
		if (!glfwInit())
			return -1;

		/* Create a windowed mode window and its OpenGL context */
		window = glfwCreateWindow(window_width, window_heith, title, NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			return -1;
		}

		/* Make the window's context current */
		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			SPDLOG_CRITICAL("Failed to init GLAD");
			return -1;
		}

		glClearColor(1, 1, 1, 0);

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();

			on_update();
		}

		glfwTerminate();
		return 0;
	}

	void Application::on_update() {

	}
}