#include "Engine/Input.h"
#include "Engine/Core.h"

Input* Input::instance;

Input::Input() {
	this->core = Core::GetInstance();
}

Input* Input::GetInstance() {
	if (Input::instance == nullptr)
		Input::instance = new Input();
	return Input::instance;
}

void Input::Callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	BYTE keyboardState[256];
	BOOL bKeyState = GetKeyboardState(keyboardState);

	if (!bKeyState)
		return;
	
	ToAscii(wParam, lParam, keyboardState, (LPWORD)&this->pressedKey, NULL);

	switch (uMsg) {
	case WM_KEYDOWN:
		this->SetKeyDown(this->pressedKey);
		break;
	case WM_KEYUP:
		this->SetKeyUp(this->pressedKey);
		break;
	case WM_LBUTTONDOWN:
		this->SetButtonDown(LEFT);
		break;
	case WM_LBUTTONUP:
		this->SetButtonUp(LEFT);
		break;
	case WM_RBUTTONDOWN:
		this->SetButtonDown(RIGHT);
		break;
	case WM_RBUTTONUP:
		this->SetButtonUp(RIGHT);
		break;
	}
}

void Input::SetKey(char key, INPUT_STATE state) {
	if (this->keys.count(key) > 0) {
		if (this->keys[key] == state)
			return;
	}

	this->keys[key] = state;

	return;
}

void Input::SetKeyDown(char key) {
	this->SetKey(key, PRESSED);
	return;
}

void Input::SetKeyUp(char key) {
	this->SetKey(key, RELEASED);
	return;
}

bool Input::GetKey(char key, INPUT_STATE state) {
	if (this->keys.count(key) < 1) 
		return false;

	if (keys[key] == state)
		return true;

	return false;
}

bool Input::GetKeyDown(char key) {
	return this->GetKey(key, PRESSED);
}

bool Input::GetKeyUp(char key) {
	return this->GetKey(key, RELEASED);
}

void Input::SetButton(MOUSE_BUTTON btn, INPUT_STATE state) {
	if (this->buttons.count(btn) > 0) {
		if (this->buttons[btn] == state)
			return;
	}

	this->buttons[btn] = state;
	return;
}

void Input::SetButtonDown(MOUSE_BUTTON btn) {
	this->SetButton(btn, PRESSED);
	return;
}

void Input::SetButtonUp(MOUSE_BUTTON btn) {
	this->SetButton(btn, RELEASED);
	return;
}

bool Input::GetButton(MOUSE_BUTTON btn, INPUT_STATE state) {
	if (this->buttons.count(btn) < 1)
		return false;

	if (buttons[btn] == state)
		return true;

	return false;
}

bool Input::GetButtonDown(MOUSE_BUTTON btn) {
	return this->GetButton(btn, PRESSED);
}

bool Input::GetButtonUp(MOUSE_BUTTON btn) {
	return this->GetButton(btn, RELEASED);
}

void Input::Close() {
	std::vector<char> keysToRemove;
	for (std::pair<char, INPUT_STATE> key : this->keys) {
		if (key.second == RELEASED)
			keysToRemove.push_back(key.first);
	}

	std::vector<MOUSE_BUTTON> btnsToRemove;
	for (std::pair<MOUSE_BUTTON, INPUT_STATE> btn : this->buttons) {
		if (btn.second == RELEASED)
			btnsToRemove.push_back(btn.first);
	}

	for (char key : keysToRemove)
		this->keys.erase(key);

	for (MOUSE_BUTTON btn : btnsToRemove)
		this->buttons.erase(btn);

	return;
}

void Input::SetHWND(HWND& hwnd) {
	this->hwnd = hwnd;
	return;
}