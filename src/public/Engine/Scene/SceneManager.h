#pragma once
#include <iostream>
#include <map>
#include "Engine/Scene/Scene.h"
#include "Engine/GameObject/Component/Component.h"

class Core;

class SceneManager {
private:
	std::map<std::string, Scene*> scenes;
	Scene* actualScene;
	
	Core* core;

	HWND hwnd;
public:
	SceneManager();

	void Start();

	void AddScene(Scene* scene);
	void SetActualScene(std::string name);

	bool SceneExists(std::string name);
	Scene* GetActualScene();

	void Update();
};