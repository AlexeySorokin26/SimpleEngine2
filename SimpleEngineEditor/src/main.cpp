#include <iostream>
#include <memory>
#include <imgui/imgui.h>

#include "SimpleEngineCore/Application.h"

class SimpleEngineEditor : public SimpleEngine::Application {
	virtual void on_update() override {
		//std::cout << "Update frame: " << frame++ << std::endl;
	}

	virtual void on_ui_draw() override {
		ImGui::Begin("Editor");
		ImGui::SliderFloat3("Cam pos", camera_pos, -10.f, 10.f);
		ImGui::SliderFloat3("Cam rotation", camera_rotation, 0.f, 360);
		ImGui::Checkbox("Perspective camera", &perspective_camera);
		ImGui::End();
	}

	int frame = 0;
};


int main() {
	auto myApp = std::make_unique<SimpleEngineEditor>();

	int returnCode = myApp->start(1024, 768, "My first app");

	return returnCode;
}