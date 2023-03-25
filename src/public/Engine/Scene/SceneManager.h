#pragma once
#include <iostream>
#include <map>
#include "Engine/Scene/Scene.h"

class Core;

class SceneManager {
private:
	std::map<std::string, Scene*> scenes;
	Scene* actualScene;
	
	Core* core;

	HWND hwnd;
public:
	SceneManager();

	void AddScene(Scene* scene);
	void SetActualScene(std::string name);

	bool SceneExists(std::string name);

	void Update();
};