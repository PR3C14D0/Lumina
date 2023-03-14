#include "Engine/ScreenQuad.h"
#include "Engine/Core.h"

ScreenQuad::ScreenQuad() {
	this->core = Core::GetInstance();
	this->core->GetWindowSize(this->width, this->height);
	this->hwnd = this->core->GetHWND();

	this->rtvIndex = this->core->GetNewHeapIndex(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	this->core->GetDeviceAndCommandList(this->dev, this->list);

	this->rtvIncrementSize = this->core->GetDescriptorIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUHandle = this->core->GetDescriptorCPUHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	this->cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvCPUHandle, this->rtvIndex, this->rtvIncrementSize);

	D3D12_RESOURCE_DESC resourceDesc = { };
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = this->core->GetMultiSampleCount();
	resourceDesc.Width = this->width;
	resourceDesc.Height = this->height;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	
	D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	D3D12_CLEAR_VALUE clearValue = { };
	clearValue.Color[3] = 1.f;
	clearValue.Format = resourceDesc.Format;

	ThrowIfFailed(this->dev->CreateCommittedResource(
		&heapProps, 
		D3D12_HEAP_FLAG_NONE, 
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clearValue,
		IID_PPV_ARGS(this->resource.GetAddressOf())));

	ThrowIfFailed(this->resource->SetName(L"ScreenQuad Buffer"));

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = { };
	rtvDesc.Format = resourceDesc.Format;
	rtvDesc.Texture2D.MipSlice = 1;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
	
	this->dev->CreateRenderTargetView(this->resource.Get(), &rtvDesc, this->cpuHandle);
		
	this->shader = new Shader(L"LightPass.hlsl", "VertexMain", "PixelMain");

	this->InitBuffers();

	/* Initialize our root signature */
	if (!this->InitRootSignature()) {
		MessageBox(this->hwnd, "An error occurred initializing ScreenQuad root signature", "Error", MB_OK);
		delete this;
		return;
	}

	this->InitPipeline();
}

/*!
	This method will initialize our vertex and index buffers.
*/
void ScreenQuad::InitBuffers() {
	ScreenQuadVertex vertices[] = {
		{ -1.f, 1.f, 0.f },
		{ 1.f, 1.f, 0.f },
		{ -1.f, -1.f, 0.f },
		{ 1.f, -1.f, 0.f },
	};
	
	UINT indices[] = {
		0, 1, 2,
		2, 1, 3
	};

	UINT verticesSize = sizeof(vertices);
	UINT indicesSize = sizeof(indices);

	D3D12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(verticesSize, D3D12_RESOURCE_FLAG_NONE);
	D3D12_RESOURCE_DESC indexDesc = CD3DX12_RESOURCE_DESC::Buffer(indicesSize, D3D12_RESOURCE_FLAG_NONE);

	D3D12_HEAP_PROPERTIES vbHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_HEAP_PROPERTIES indexHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	ThrowIfFailed(this->dev->CreateCommittedResource(&vbHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&vbDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(this->vertexBuffer.GetAddressOf())
	));

	ThrowIfFailed(this->dev->CreateCommittedResource(
		&indexHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&indexDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(this->indexBuffer.GetAddressOf())
	));

	PVOID map;
	this->vertexBuffer->Map(0, nullptr, &map);
	memcpy(map, vertices, verticesSize);
	this->vertexBuffer->Unmap(0, nullptr);
	map = nullptr;

	this->indexBuffer->Map(0, nullptr, &map);
	memcpy(map, indices, indicesSize);
	this->indexBuffer->Unmap(0, nullptr);

	this->vbv.BufferLocation = this->vertexBuffer->GetGPUVirtualAddress();
	this->vbv.SizeInBytes = verticesSize;
	this->vbv.StrideInBytes = sizeof(ScreenQuadVertex);

	this->ibv.BufferLocation = this->indexBuffer->GetGPUVirtualAddress();
	this->ibv.Format = DXGI_FORMAT_R32_UINT;
	this->ibv.SizeInBytes = indicesSize;

	return;
}

/*!
	This method will initialize our ScreenQuad root signature.
*/
bool ScreenQuad::InitRootSignature() {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;

	/* Get the indexes for our descriptors */
	UINT albedoIndex = this->core->GetNewHeapIndex(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	UINT positionIndex = this->core->GetNewHeapIndex(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	UINT normalIndex = this->core->GetNewHeapIndex(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	UINT cbv_srvHeapIncrementSize = this->core->GetDescriptorIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); // Get our CBV / SRV / UAV descriptor heap increment size
	D3D12_CPU_DESCRIPTOR_HANDLE cbv_srvHandle = this->core->GetDescriptorCPUHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); // Get our CBV / SRV / UAV descriptor heap handle CPU handle.
	D3D12_GPU_DESCRIPTOR_HANDLE cbv_srvGPUHandle = this->core->GetDescriptorGPUHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); // Get our CBV / SRV / UAV descriptor heap handle GPU handle.

	/* Get our SRV handles for our G-Buffers */
	D3D12_CPU_DESCRIPTOR_HANDLE albedoHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbv_srvHandle, albedoIndex, cbv_srvHeapIncrementSize);
	D3D12_CPU_DESCRIPTOR_HANDLE normalHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbv_srvHandle, normalIndex, cbv_srvHeapIncrementSize);
	D3D12_CPU_DESCRIPTOR_HANDLE positionHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbv_srvHandle, positionIndex, cbv_srvHeapIncrementSize);

	this->albedoGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbv_srvGPUHandle, albedoIndex, cbv_srvHeapIncrementSize);
	this->normalGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbv_srvGPUHandle, normalIndex, cbv_srvHeapIncrementSize);
	this->positionGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbv_srvGPUHandle, positionIndex, cbv_srvHeapIncrementSize);

	/* Get our G-Buffer resources */
	this->core->GetGBuffer(GBUFFER_TYPE::ALBEDO, this->albedo);
	this->core->GetGBuffer(GBUFFER_TYPE::POSITION, this->position);
	this->core->GetGBuffer(GBUFFER_TYPE::NORMAL, this->normals);

	this->dev->CreateShaderResourceView(this->albedo.Get(), &srvDesc, albedoHandle);
	this->dev->CreateShaderResourceView(this->position.Get(), &srvDesc, positionHandle);
	this->dev->CreateShaderResourceView(this->normals.Get(), &srvDesc, normalHandle);

	CD3DX12_DESCRIPTOR_RANGE albedoRange;
	CD3DX12_DESCRIPTOR_RANGE positionRange;
	CD3DX12_DESCRIPTOR_RANGE normalRange;

	albedoRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	normalRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	positionRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

	CD3DX12_ROOT_PARAMETER albedoParam;
	CD3DX12_ROOT_PARAMETER positionParam;

	CD3DX12_ROOT_PARAMETER normalParam;
	albedoParam.InitAsDescriptorTable(1, &albedoRange, D3D12_SHADER_VISIBILITY_PIXEL);
	normalParam.InitAsDescriptorTable(1, &normalRange, D3D12_SHADER_VISIBILITY_PIXEL);
	positionParam.InitAsDescriptorTable(1, &positionRange, D3D12_SHADER_VISIBILITY_PIXEL);

	std::vector<D3D12_ROOT_PARAMETER> rootParams;

	rootParams.push_back(albedoParam);
	rootParams.push_back(normalParam);
	rootParams.push_back(positionParam);

	ComPtr<ID3DBlob> rootSigBlob, rootSigErr;

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = { };
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSigDesc.NumParameters = rootParams.size();
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pParameters = rootParams.data();
	rootSigDesc.pStaticSamplers = nullptr;

	ThrowIfFailed(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, rootSigBlob.GetAddressOf(), rootSigErr.GetAddressOf()));

	if (rootSigErr) {
		MessageBox(this->hwnd, (const char*)rootSigErr->GetBufferPointer(), "Error", MB_OK);
		return false;
	}

	ThrowIfFailed(this->dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(this->rootSig.GetAddressOf())));

	return true;
}

/*!
	This method initializes our ScreenQuad graphics pipeline.
*/
void ScreenQuad::InitPipeline() {
	D3D12_INPUT_ELEMENT_DESC elements[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, NULL}
	};

	UINT nNumElements = _countof(elements);

	ZeroMemory(&this->layoutDesc, sizeof(D3D12_INPUT_LAYOUT_DESC));

	this->layoutDesc.NumElements = nNumElements;
	this->layoutDesc.pInputElementDescs = elements;
	
	ComPtr<ID3DBlob> VS, PS;
	this->shader->GetBlob(VS, PS);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC plDesc = { };
	plDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	plDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	plDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	plDesc.pRootSignature = this->rootSig.Get();
	plDesc.SampleDesc.Count = this->core->GetMultiSampleCount();
	plDesc.SampleMask = UINT32_MAX;
	plDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	plDesc.VS = CD3DX12_SHADER_BYTECODE(VS.Get());
	plDesc.PS = CD3DX12_SHADER_BYTECODE(PS.Get());
	plDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
	plDesc.InputLayout = this->layoutDesc;
	plDesc.NumRenderTargets = 1;
	plDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	plDesc.DepthStencilState.DepthEnable = FALSE;
	plDesc.DepthStencilState.StencilEnable = FALSE;
	
	ThrowIfFailed(this->dev->CreateGraphicsPipelineState(&plDesc, IID_PPV_ARGS(this->plState.GetAddressOf())));
}

/*!
	Our ScreenQuad render method. This method will be called once per frame.
*/
void ScreenQuad::Render() {
	/* Transition */
	std::vector<D3D12_RESOURCE_BARRIER> barriers;
	std::vector<ID3D12Resource*> resources;
	resources.push_back(this->albedo.Get());
	resources.push_back(this->normals.Get());
	resources.push_back(this->position.Get());

	for (ID3D12Resource* pResource : resources) {
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(pResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		barriers.push_back(barrier);
	}

	this->list->ResourceBarrier(barriers.size(), barriers.data());

	/* Render */
	this->list->OMSetRenderTargets(1, &this->cpuHandle, FALSE, nullptr);
	this->list->ClearRenderTargetView(this->cpuHandle, RGBA{0.f, 0.f, 0.f, 1.f}, 0, nullptr);
	this->list->IASetVertexBuffers(0, 1, &this->vbv);
	this->list->SetPipelineState(this->plState.Get());
	this->list->IASetIndexBuffer(&this->ibv);
	this->list->SetGraphicsRootSignature(this->rootSig.Get());
	this->list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->list->SetGraphicsRootDescriptorTable(0, this->albedoGPUHandle);
	this->list->SetGraphicsRootDescriptorTable(1, this->normalGPUHandle);
	this->list->SetGraphicsRootDescriptorTable(2, this->positionGPUHandle);

	this->list->DrawIndexedInstanced(6, 1, 0, 0, 0);

	/* Transition */
	barriers.clear();

	for (ID3D12Resource* pResource : resources) {
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(pResource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		barriers.push_back(barrier);
	}

	this->list->ResourceBarrier(barriers.size(), barriers.data());
}