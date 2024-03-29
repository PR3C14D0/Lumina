#include "Engine/Scene/Scene.h"
#include "Engine/Core.h"

Scene::Scene(std::string name) {
	this->name = name;
	this->core = Core::GetInstance();
	this->hwnd = this->core->GetHWND();
}

/*!
	This method will set the specified GameObject as a camera.
		Note: The GameObject must be an instance of Camera and must be added on scene.
*/
void Scene::SetCamera(std::string cameraName) {
	if (this->gameObjects.count(cameraName) > 0) {
		GameObject* obj = this->gameObjects[cameraName];
		Camera* camera = dynamic_cast<Camera*>(obj);
		if (camera) {
			this->actualCamera = camera;
		}
		else {
			MessageBox(this->hwnd, "That GameObject is not a Camera.", "Error", MB_OK);
		}
	}
	else {
		MessageBox(this->hwnd, "No GameObject with that name is placed on the scene.", "Error", MB_OK);
	}
}

/*!
	Our Scene main loop.
		Note: This method will be called once per frame.
*/
void Scene::Update() {
	/* Update our objects */
	for (std::pair<std::string, GameObject*> obj : this->gameObjects)
		obj.second->Update();
}

Camera* Scene::GetCamera() {
	return this->actualCamera;
}