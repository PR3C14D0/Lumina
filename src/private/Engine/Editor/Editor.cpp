#include "Engine/Editor/Editor.h"

Editor* Editor::instance;

Editor::Editor() {
	this->time = Time::GetInstance();
	this->bEditOpen = false;
}

void Editor::Start() {
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.16f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.22f, 0.22f, 0.22f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.16f, 0.16f, 0.16f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 0.31f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.22f, 0.22f, 0.22f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.42f, 0.42f, 0.42f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.16f, 0.16f, 0.84f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 0.84f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.16f, 0.16f, 0.16f, 0.84f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.22f, 0.22f, 0.22f, 0.67f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.16f, 0.16f, 0.94f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 0.50f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 0.78f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.22f, 0.54f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 0.67f);
	colors[ImGuiCol_Header] = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.67f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.42f, 0.42f, 0.42f, 0.67f);
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.41f, 0.42f, 0.44f, 0.78f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.22f, 0.22f, 0.22f, 0.54f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.67f);
	colors[ImGuiCol_TabActive] = ImVec4(0.42f, 0.42f, 0.42f, 0.67f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.22f, 0.22f, 0.22f, 0.54f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.22f, 0.22f, 0.22f, 0.54f);
	/*colors[ImGuiCol_DockingPreview] = ImVec4(0.51f, 0.51f, 0.51f, 0.70f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.16f, 0.16f, 0.16f, 0.94f);*/
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	style->WindowRounding = 4.0f;
	style->FrameRounding = 2.0f;
	style->ScrollbarRounding = 4.0f;
	style->GrabRounding = 2.0f;
	style->PopupRounding = 4.0f;
}

void Editor::Update() {
	this->MenuBar();
	this->Edit();
	this->Performance();
}

void Editor::MenuBar() {
	ImGui::BeginMainMenuBar();
	ImGui::MenuItem("File");
	if (ImGui::MenuItem("Edit"))
		this->bEditOpen = !this->bEditOpen;
	ImGui::MenuItem("GameObject");
	ImGui::EndMainMenuBar();
}

void Editor::Edit() {
	if (this->bEditOpen) {
		ImGui::Begin("Edit", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::Button("Settings");
		ImGui::Button("Project settings");
		ImGui::End();
	}
}

void Editor::Performance() {
	ImGui::Begin("Performance");
	ImGui::Text(("FPS: " + std::to_string((int)(1.f / this->time->deltaTime))).c_str());
	ImGui::End();
}

Editor* Editor::GetInstance() {
	if (Editor::instance == nullptr)
		Editor::instance = new Editor();
	return Editor::instance;
}