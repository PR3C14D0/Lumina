#pragma once
#include <iostream>
#include <string>
#include <imgui/imgui.h>
#include "Engine/Time.h"
#include <map>
#include "Engine/GameObject/GameObject.h"

class Core;

class Editor {
private:
	Core* core;
	SceneManager* sceneMgr;
	static Editor* instance;
	Time* time;

	bool bEditOpen;

	void Performance();
	void Edit();
	void MenuBar();

	void Hierarchy();
	void Properties();

	GameObject* workingObj;
	std::map<std::string, GameObject*> objs;
public:
	Editor();
	static Editor* GetInstance();
	
	void Start();
	void Update();
};