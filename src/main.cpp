#include <Windows.h>
#include "Engine/Core.h"

bool g_quit = false;
Core* g_core = Core::GetInstance();

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	const char CLASS_NAME[] = "LuminaEngine";

	WNDCLASS wc = { };
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		NULL,
		CLASS_NAME,
		"Lumina Engine",
		WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,

		NULL, NULL, hInstance, NULL
	);

	if (!hwnd) {
		MessageBox(NULL, "An error occurred creating the window", "Error", MB_OK);
		return TRUE;
	}

	ShowWindow(hwnd, SW_MAXIMIZE);

	g_core->SetHWND(hwnd);
	g_core->Start();

	MSG msg = { };

	while (!g_quit) {
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		g_core->MainLoop();
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		g_quit = true;
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}