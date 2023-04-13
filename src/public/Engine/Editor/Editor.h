#pragma once
#include <iostream>
#include <string>
#include <imgui/imgui.h>
#include "Engine/Time.h"

class Editor {
private:
	static Editor* instance;
	Time* time;
public:
	Editor();
	static Editor* GetInstance();
	
	void Update();
};