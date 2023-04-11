#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <Windows.h>

/* Lumina Engine's "InputWave" input system */

enum INPUT_STATE {
	PRESSED = 0,
	RELEASED = 1
};

enum MOUSE_BUTTON {
	LEFT = 0,
	RIGHT = 1
};

class Input {
private:
	static Input* instance;

	std::map<char, INPUT_STATE> keys;
	std::map<MOUSE_BUTTON, INPUT_STATE> buttons;

	char pressedKey;

	HWND hwnd;
	int centerX, centerY;

	HCURSOR hCursor;
public:
	Input();
	static Input* GetInstance();
	void Callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Close();

	void SetKey(char key, INPUT_STATE state);
	void SetKeyDown(char key);
	void SetKeyUp(char key);

	bool GetKey(char key, INPUT_STATE state);
	bool GetKeyDown(char key);
	bool GetKeyUp(char key);

	void SetButton(MOUSE_BUTTON btn, INPUT_STATE state);
	void SetButtonDown(MOUSE_BUTTON btn);
	void SetButtonUp(MOUSE_BUTTON btn);

	bool GetButton(MOUSE_BUTTON btn, INPUT_STATE state);
	bool GetButtonDown(MOUSE_BUTTON btn);
	bool GetButtonUp(MOUSE_BUTTON btn);

	void SetHWND(HWND& hwnd);

	void ShowCursor(bool bShow);
	float deltaX, deltaY;
};