#pragma once
#include <iostream>
#include <map>
#include "Engine/Camera/Camera.h"

class Scene {
private:
	std::string name;
	std::map<std::string, GameObject*> gameObjects;

	Camera* actualCamera;
public:
	Scene(std::string name);

	void SetCamera(std::string cameraName);
};