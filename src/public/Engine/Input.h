#pragma once
#include <iostream>
#include <map>

class Input {
private:
	static Input* instance;
public:
	Input();
	static Input* GetInstance();
};