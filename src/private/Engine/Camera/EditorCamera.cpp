#include "Engine/Camera/EditorCamera.h"

EditorCamera::EditorCamera(std::string name) : Camera::Camera(name) {

}

void EditorCamera::Start() {
	return;
}

void EditorCamera::Update() {
	Camera::Update();
	std::cout << "[X] " << this->input->deltaX << " [Y] " << this->input->deltaY << std::endl;
	if (this->input->GetButtonDown(RIGHT)) {
		this->input->ShowCursor(false);
		float deltaX = this->input->deltaX;
		float deltaY = this->input->deltaY;
		float sensX = 0.1f;
		float sensY = 0.1f;

		this->transform.rotate(deltaY * sensY, deltaX * sensX, 0.f);

		if (this->input->GetKeyDown('w')) {
			Vector3 translation = this->transform.Forward() * 1.f * time->deltaTime;
			this->transform.translate(translation);
		}
		if (this->input->GetKeyDown('s')) {
			Vector3 translation = this->transform.Forward() * -1.f * time->deltaTime;
			this->transform.translate(translation);
		}
		if (this->input->GetKeyDown('d')) {
			Vector3 translation = this->transform.Right() * 1.f * time->deltaTime;
			this->transform.translate(translation);
		}
		if (this->input->GetKeyDown('a')) {
			Vector3 translation = this->transform.Right() * -1.f * time->deltaTime;
			this->transform.translate(translation);
		}
	}
	else {
		this->input->ShowCursor(true);
	}
	
	return;
}