#include "Engine/Editor/Editor.h"

Editor* Editor::instance;

Editor::Editor() {
	this->time = Time::GetInstance();
}

void Editor::Update() {
	ImGui::Begin("Performance");
	ImGui::Text(("FPS: " + std::to_string((int)(1.f / this->time->deltaTime))).c_str());
	ImGui::End();
}

Editor* Editor::GetInstance() {
	if (Editor::instance == nullptr)
		Editor::instance = new Editor();
	return Editor::instance;
}