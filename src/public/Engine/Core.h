#pragma once
#include <iostream>
#include <vector>
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
	ComPtr<ID3D12CommandAllocator> alloc;
	ComPtr<ID3D12CommandQueue> queue;

	void GetMostCapableAdapter(ComPtr<IDXGIFactory2>& factory, ComPtr<IDXGIAdapter>& adapter);
	D3D_FEATURE_LEVEL GetMaxFeatureLevel(ComPtr<IDXGIAdapter>& adapter);
public:
	void SetHWND(HWND& hwnd);

	void Init();

	Core();
	static Core* GetInstance();
};