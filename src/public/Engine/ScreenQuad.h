#pragma once
#include <iostream>
#include <vector>
#include <Windows.h>
#include <wrl.h>
#include "DirectXIncludes.h"
#include "Engine/Util.h"
#include "Engine/Vertex.h"
#include "Engine/Shader/Shader.h"

using namespace Microsoft::WRL;

class Core;

class ScreenQuad {
	friend Core;
private:
	UINT rtvIncrementSize;
	UINT rtvIndex;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;

	ComPtr<ID3D12Resource> resource;

	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbv;
	D3D12_INDEX_BUFFER_VIEW ibv;

	ComPtr<ID3D12PipelineState> plState;
	ComPtr<ID3D12RootSignature> rootSig;

	ComPtr<ID3D12Device> dev;
	ComPtr<ID3D12GraphicsCommandList> list;

	D3D12_GPU_DESCRIPTOR_HANDLE albedoGPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE normalGPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE positionGPUHandle;
	
	ComPtr<ID3D12Resource> albedo, position, normals;

	int width, height;

	Core* core;
	HWND hwnd;

	D3D12_INPUT_LAYOUT_DESC layoutDesc;

	void InitBuffers();

	bool InitRootSignature();
	void InitPipeline();

	Shader* shader;

public:
	ScreenQuad();

	void Render();
};