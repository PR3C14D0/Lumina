#include "Engine/Scene/Scene.h"
#include "Engine/Core.h"

Scene::Scene(std::string name) {
	this->name = name;
	this->core = Core::GetInstance();
	this->hwnd = this->core->GetHWND();
	this->editorCamera = new EditorCamera("Editor Camera");
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
	This method sets our actual camera as editor camera.
*/
void Scene::UseEditorCamera() {
	this->actualCamera = this->editorCamera;
	return;
}

/*!
	Our Scene main loop.
		Note: This method will be called once per frame.
*/
void Scene::Update() {
	/* Update our objects */
	for (std::pair<std::string, GameObject*> obj : this->gameObjects)
		obj.second->Update();

	this->editorCamera->Update();
}

/*!
	This method will return our scene GameObjects.
*/
void Scene::GetObjects(std::map<std::string, GameObject*>& objs) {
	objs = this->gameObjects;
}

Camera* Scene::GetCamera() {
	return this->actualCamera;
}