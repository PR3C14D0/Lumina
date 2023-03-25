#pragma once
#include <iostream>
#include <Windows.h>
#include <map>
#include "Engine/Camera/Camera.h"

class Core; /* Core class forward declaration */

class Scene {
	friend class SceneManager;
private:
	Core* core;
	HWND hwnd;

	std::string name;
	std::map<std::string, GameObject*> gameObjects;

	Camera* actualCamera;
public:
	Scene(std::string name);

	void SetCamera(std::string cameraName);
	void Update();
};