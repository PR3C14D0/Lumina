#include <Windows.h>
#include "Engine/Core.h"
#include "Engine/Input.h"
#include "Engine/Time.h"
#include <time.h>

bool g_quit = false;
Input* g_input = Input::GetInstance();
Core* g_core = Core::GetInstance();
Time* g_time = Time::GetInstance();

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

	g_input->SetHWND(hwnd);
	g_core->SetHWND(hwnd);
	g_core->Start();

	clock_t startTime = clock();
	float deltaTime;
	clock_t endTime;

	MSG msg = { };

	while (!g_quit) {
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		g_core->MainLoop();
		endTime = clock();
		deltaTime = endTime - startTime;
		g_time->SetDelta(deltaTime / 1000.f);
		startTime = endTime;
	}
	return FALSE;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		return 0;

	g_input->Callback(hwnd, uMsg, wParam, lParam);

	switch (uMsg) {
	case WM_DESTROY:
		g_quit = true;
		PostQuitMessage(0);
		return 0;
	}

	g_input->Close();

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}