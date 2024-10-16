#include "UIModule.h"


#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <GLFW/glfw3.h>

namespace SimpleEngine {

	void UIModule::on_window_create(GLFWwindow* pWindow)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// set window size where imgui should draw
		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig font_config;
		font_config.SizePixels = 30.0f; // Установите размер шрифта здесь
		io.Fonts->AddFontDefault(&font_config);
		io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_ViewportsEnable;

		ImGui_ImplOpenGL3_Init();
		ImGui_ImplGlfw_InitForOpenGL(pWindow, true);
	}

	void UIModule::on_window_close()
	{
		// Shutdown ImGui backends first
		ImGui_ImplGlfw_Shutdown();      // Shutdown the GLFW backend
		ImGui_ImplOpenGL3_Shutdown();   // Shutdown the OpenGL backend

		// Then destroy the ImGui context
		if (ImGui::GetCurrentContext()) {
			ImGui::DestroyContext();
		}
	}

	void UIModule::on_ui_draw_begin()
	{
		// create frame for imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void UIModule::on_ui_draw_end()
	{
		// render it in imgui
		ImGui::Render();
		// use opengl to draw
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// this part is smth extra since it was in example
		// but maybe used for macos
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
}
