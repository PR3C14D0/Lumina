#include "Engine/Input.h"

Input* Input::instance;

Input* Input::GetInstance() {
	if (Input::instance == nullptr)
		Input::instance = new Input();
	return Input::instance;
}