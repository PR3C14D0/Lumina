#pragma once
#include <iostream>
#include <Windows.h>
#include <map>
#include "Engine/Camera/Camera.h"

class Core; /* Core class forward declaration */

class Scene {
private:
	Core* core;
	HWND hwnd;

	std::string name;
	std::map<std::string, GameObject*> gameObjects;

	Camera* actualCamera;
public:
	Scene(std::string name);

	void SetCamera(std::string cameraName);
};