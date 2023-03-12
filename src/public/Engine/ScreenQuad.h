#pragma once
#include <iostream>
#include <vector>
#include <Windows.h>
#include <wrl.h>
#include "DirectXIncludes.h"
#include "Engine/Util.h"
#include "Engine/Vertex.h"

using namespace Microsoft::WRL;

class Core;

class ScreenQuad {
private:
	UINT rtvIncrementSize;
	UINT rtvIndex;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;

	ComPtr<ID3D12Resource> resource;

	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbv;
	D3D12_INDEX_BUFFER_VIEW ibv;

	ComPtr<ID3D12PipelineLibrary> plState;
	ComPtr<ID3D12RootSignature> rootSig;

	ComPtr<ID3D12Device> dev;
	ComPtr<ID3D12GraphicsCommandList> list;
	
	int width, height;

	Core* core;
	HWND hwnd;

	D3D12_INPUT_LAYOUT_DESC layoutDesc;

	void InitBuffers();

	bool InitRootSignature();
	void InitInputLayout();
	void InitPipeline();

public:
	ScreenQuad();

	void Render();
};