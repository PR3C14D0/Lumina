#include "Engine/Scene/Scene.h"

Scene::Scene(std::string name) {
	this->name = name;
}

void Scene::SetCamera(std::string cameraName) {
	if (this->gameObjects.count(cameraName) > 0) {
		GameObject* obj = this->gameObjects[cameraName];
		Camera* camera = dynamic_cast<Camera*>(obj);
		if (camera) {
			this->actualCamera = camera;
		}
		else {
			/* TODO: Throw an error message or anything. */
		}
	}
}