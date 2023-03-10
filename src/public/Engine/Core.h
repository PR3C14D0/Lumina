#pragma once
#include <iostream>
#include <vector>
#include <Windows.h>
#include <wrl.h>
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <dxgi1_4.h>
#include "Engine/Util.h"
#include <map>

using namespace Microsoft::WRL;

enum GBUFFER_TYPE {
	ALBEDO = 0,
	NORMAL = 1,
	POSITION = 2,

	GBUFFER_TYPE_LENGTH
};

class Core {
private:
	static Core* instance;
	HWND hwnd;

	ComPtr<IDXGIFactory2> factory;
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<IDXGISwapChain3> sc;

	ComPtr<ID3D12Device> dev;
	ComPtr<ID3D12CommandAllocator> alloc;
	ComPtr<ID3D12CommandQueue> queue;
	ComPtr<ID3D12GraphicsCommandList> list;
	
	UINT nNumBackBuffers;
	UINT nNumRenderTargets;
	UINT nNumFBO;
	UINT nNumGBuffers;
	UINT nNumOtherFBOs;

	int width, height;

	void GetMostCapableAdapter(ComPtr<IDXGIFactory2>& factory, ComPtr<IDXGIAdapter>& adapter);
	D3D_FEATURE_LEVEL GetMaxFeatureLevel(ComPtr<IDXGIAdapter>& adapter);

	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> samplerHeap;
	ComPtr<ID3D12DescriptorHeap> cbv_srvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;

	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	std::map<GBUFFER_TYPE, ComPtr<ID3D12Resource>> FBOs;
	std::map<GBUFFER_TYPE, UINT> gbufferIndices;

	UINT rtvActualIndex;
	UINT samplerActualIndex;
	UINT cbv_srvActualIndex;
	UINT dsvActualIndex;

	UINT nRTVHeapIncrementSize;
	UINT nSamplerHeapIncrementSize;
	UINT nCBV_SRVHeapIncrementSize;
	UINT nDSVHeapIncrementSize;

	UINT nMultisampleCount;

	void InitDescriptorHeaps();
public:
	void SetHWND(HWND& hwnd);

	UINT GetNewHeapIndex(D3D12_DESCRIPTOR_HEAP_TYPE type);

	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorCPUHandle(D3D12_DESCRIPTOR_HEAP_TYPE type);
	D3D12_GPU_DESCRIPTOR_HANDLE GetDescriptorGPUHandle(D3D12_DESCRIPTOR_HEAP_TYPE type);
	UINT GetDescriptorIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type);

	void Init();

	Core();
	static Core* GetInstance();
};