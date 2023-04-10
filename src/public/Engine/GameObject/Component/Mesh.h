#pragma once
#include <iostream>
#include <vector>
#include "Engine/GameObject/Component/Component.h"
#include "Math/Transform.h"
#include "DirectXIncludes.h"
#include "Engine/Shader/Shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Engine/Vertex.h"
#include <dxtk/ResourceUploadBatch.h>
#include <dxtk/WICTextureLoader.h>
#include "Engine/ConstantBuffers.h"

class Core;
class SceneManager;

class Mesh : public Component {
private:
	Core* core;
	SceneManager* sceneMgr;

	ComPtr<ID3D12Device> dev;
	ComPtr<ID3D12GraphicsCommandList> list;
	ComPtr<ID3D12CommandQueue> queue;
	
	ComPtr<ID3D12Resource> texture; // TODO: Allow multiple textures.
	ComPtr<ID3D12Resource> VBO; // TODO: Allow multiple vertex buffers.
	D3D12_VERTEX_BUFFER_VIEW VBV;

	ComPtr<ID3D12RootSignature> rootSig;

	ComPtr<ID3D12PipelineState> plState;

	std::vector<Vertex> vertices;

	Shader* shader;
	void InitPipeline();

	void UploadBuffers();
	void Draw();

	Transform* transform;

	WVP wvp;
	ComPtr<ID3D12Resource> cbuffer;
	UINT nWVPIndex;
	D3D12_CPU_DESCRIPTOR_HANDLE cbvCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE cbvGPUHandle;
	UINT nCBVHeapIncrementSize;

	void InitCBuffer();
	void UpdateCBuffer();
public:
	Mesh(Transform* parentTransform);

	void Start() override;
	void Update() override;

	bool bMeshLoaded;

	void LoadFromFile(std::string file);
};