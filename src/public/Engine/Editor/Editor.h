#pragma once
#include <iostream>
#include <string>
#include <imgui/imgui.h>
#include "Engine/Time.h"

class Editor {
private:
	static Editor* instance;
	Time* time;

	bool bEditOpen;
public:
	Editor();
	static Editor* GetInstance();
	
	void Performance();
	void Edit();
	void MenuBar();

	void Start();
	void Update();
};