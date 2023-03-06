#pragma once
#include <Windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include "Engine/Util.h"

using namespace Microsoft::WRL;

class Core {
private:
	static Core* instance;

	HWND hwnd;
public:
	void SetHWND(HWND& hwnd);

	void Init();

	Core();
	static Core* GetInstance();
};