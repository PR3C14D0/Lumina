#pragma once
#include <iostream>
#include <vector>
#include <map>
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
#include "Engine/ResourceManager.h"
#include <execution>
#include <algorithm>

class Core;
class SceneManager;

class Mesh : public Component {
private:
	Core* core;
	SceneManager* sceneMgr;

	ComPtr<ID3D12Device> dev;
	ComPtr<ID3D12GraphicsCommandList> list;
	ComPtr<ID3D12CommandQueue> queue;
	
	ComPtr<ID3D12Resource> texture;
	UINT nSamplerIndex;

	std::vector<ComPtr<ID3D12Resource>> VBOs;
	std::vector<D3D12_VERTEX_BUFFER_VIEW> VBVs;

	ComPtr<ID3D12RootSignature> rootSig;

	std::map<UINT, std::string> textureIndices;
	std::map<std::string, UINT> textureSrvIndices;

	ComPtr<ID3D12PipelineState> plState;

	std::map<UINT, std::vector<Vertex>> vertices;

	Shader* shader;
	void InitPipeline();

	void UploadBuffers();
	void Draw();

	Transform* transform;

	WVP wvp;
	ComPtr<ID3D12Resource> cbuffer;
	UINT nWVPIndex;

	D3D12_CPU_DESCRIPTOR_HANDLE cbv_srvCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE cbv_srvGPUHandle;
	UINT nCBVHeapIncrementSize;

	D3D12_CPU_DESCRIPTOR_HANDLE samplerCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE samplerGPUHandle;
	UINT nSamplerIncrementSize;

	void InitCBuffer();
	void UpdateCBuffer();
	void PrepareTextures();

	ResourceManager* resMgr;
public:
	Mesh(Transform* parentTransform);

	void Start() override;
	void Update() override;

	bool bMeshLoaded;

	void LoadFromFile(std::string file);
};