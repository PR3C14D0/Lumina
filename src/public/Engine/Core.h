#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include "Engine/Util.h"

using namespace Microsoft::WRL;

class Core {
private:
	static Core* instance;
	HWND hwnd;

	ComPtr<IDXGIFactory2> factory;
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<IDXGISwapChain1> sc;
	ComPtr<ID3D12Device> dev;
public:
	void SetHWND(HWND& hwnd);

	void Init();

	Core();
	static Core* GetInstance();
};