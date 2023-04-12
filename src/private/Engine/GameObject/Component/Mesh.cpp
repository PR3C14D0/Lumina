#include "Engine/GameObject/Component/Mesh.h"
#include "Engine/Core.h"
#include "Engine/Scene/SceneManager.h"

Mesh::Mesh(Transform* parentTransform) : Component::Component() {
	this->core = Core::GetInstance();
	this->core->GetDeviceAndCommandList(this->dev, this->list);
	this->core->GetCommandQueue(this->queue);

	this->bMeshLoaded = false;

	this->transform = parentTransform;

	/*  WVP Initialization for this mesh. */
	Vector3 location = this->transform->location;
	Vector3 rotation = this->transform->rotation;
	this->wvp.World = XMMatrixTranspose(XMMatrixIdentity() * XMMatrixTranslation(location.x, location.y, location.z));
	this->wvp.World *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(rotation.x)));
	this->wvp.World *= XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(rotation.z)));
	this->wvp.World *= XMMatrixTranspose(XMMatrixRotationZ(XMConvertToRadians(rotation.y)));
   
	this->wvp.View = XMMatrixTranspose(XMMatrixIdentity());
	this->wvp.Projection = XMMatrixTranspose(XMMatrixIdentity());

	this->cbv_srvCPUHandle = this->core->GetDescriptorCPUHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	this->cbv_srvGPUHandle = this->core->GetDescriptorGPUHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	this->nCBVHeapIncrementSize = this->core->GetDescriptorIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	this->samplerCPUHandle = this->core->GetDescriptorCPUHandle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	this->samplerGPUHandle = this->core->GetDescriptorGPUHandle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	this->nSamplerIncrementSize = this->core->GetDescriptorIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Mesh::Start() {
	Component::Start();
	this->sceneMgr = this->core->GetSceneManager();
	if (this->bMeshLoaded) {
		this->InitCBuffer();
		this->UploadBuffers();
		this->InitPipeline();
		this->PrepareTextures();
	}
}


void Mesh::Update() {
	Component::Update();

	if (this->bMeshLoaded) {
		this->UpdateCBuffer();
		this->Draw();
	}
}

void Mesh::InitCBuffer() {
	UINT alignedCBuffSize = (sizeof(this->wvp) + 255) & ~255;

	D3D12_HEAP_PROPERTIES cbuffProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC cbuffResDesc = CD3DX12_RESOURCE_DESC::Buffer(alignedCBuffSize);

	ThrowIfFailed(this->dev->CreateCommittedResource(&cbuffProps, D3D12_HEAP_FLAG_NONE, &cbuffResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(this->cbuffer.GetAddressOf())));

	PVOID ms;
	this->cbuffer->Map(NULL, nullptr, &ms);
	memcpy(ms, &this->wvp, alignedCBuffSize);
	this->cbuffer->Unmap(NULL, nullptr);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv = { };
	cbv.BufferLocation = this->cbuffer->GetGPUVirtualAddress();
	cbv.SizeInBytes = alignedCBuffSize;

	this->nWVPIndex = this->core->GetNewHeapIndex(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_CPU_DESCRIPTOR_HANDLE cbuffHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(this->cbv_srvCPUHandle, this->nWVPIndex, nCBVHeapIncrementSize);

	this->dev->CreateConstantBufferView(&cbv, cbuffHandle);

	return;
}

void Mesh::InitPipeline() {
	CD3DX12_DESCRIPTOR_RANGE wvpRange;
	CD3DX12_DESCRIPTOR_RANGE samplerRange;
	CD3DX12_DESCRIPTOR_RANGE textureRange;

	wvpRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0);
	samplerRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0, 0);
	textureRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

	CD3DX12_ROOT_PARAMETER wvpParam;
	CD3DX12_ROOT_PARAMETER samplerParam;
	CD3DX12_ROOT_PARAMETER textureParam;
	wvpParam.InitAsDescriptorTable(1, &wvpRange, D3D12_SHADER_VISIBILITY_VERTEX);
	samplerParam.InitAsDescriptorTable(1, &samplerRange, D3D12_SHADER_VISIBILITY_PIXEL);
	textureParam.InitAsDescriptorTable(1, &textureRange, D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_PARAMETER params[] = {
		wvpParam,
		samplerParam,
		textureParam
	};

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = { };
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSigDesc.NumParameters = _countof(params);
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pParameters = params;
	rootSigDesc.pStaticSamplers = nullptr;

	ComPtr<ID3DBlob> rsBlob, rsErr;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, rsBlob.GetAddressOf(), rsErr.GetAddressOf()));

	if (rsErr) {
		MessageBox(NULL, (char*)rsErr->GetBufferPointer(), "Error", MB_OK);
		return;
	}

	ThrowIfFailed(this->dev->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(this->rootSig.GetAddressOf())));

	D3D12_INPUT_ELEMENT_DESC elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, NULL },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, NULL },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, NULL },
	};

	D3D12_INPUT_LAYOUT_DESC layout = { };
	layout.pInputElementDescs = elements;
	layout.NumElements = _countof(elements);

	this->shader = new Shader(L"GBufferPass.hlsl", "VertexMain", "PixelMain");
	ComPtr<ID3DBlob> VS, PS;
	this->shader->GetBlob(VS, PS);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC plDesc = { };
	plDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	plDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	plDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	plDesc.DepthStencilState.DepthEnable = TRUE;
	plDesc.DepthStencilState.StencilEnable = FALSE;
	plDesc.pRootSignature = this->rootSig.Get();
	plDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	plDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
	plDesc.RTVFormats[1] = DXGI_FORMAT_B8G8R8A8_UNORM;
	plDesc.RTVFormats[2] = DXGI_FORMAT_B8G8R8A8_UNORM;
	plDesc.NumRenderTargets = 3;
	plDesc.InputLayout = layout;
	plDesc.VS = CD3DX12_SHADER_BYTECODE(VS.Get());
	plDesc.PS = CD3DX12_SHADER_BYTECODE(PS.Get());
	plDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	plDesc.SampleDesc.Count = this->core->GetMultiSampleCount();
	plDesc.SampleMask = UINT32_MAX;
	plDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	
	ThrowIfFailed(this->dev->CreateGraphicsPipelineState(&plDesc, IID_PPV_ARGS(this->plState.GetAddressOf())));
	this->plState->SetName(L"Mesh Pipeline State");
}

/*!
	Our mesh draw method.
*/
void Mesh::Draw() {
	this->list->SetGraphicsRootSignature(this->rootSig.Get());
	this->list->IASetVertexBuffers(0, 1, &this->VBV);
	this->list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->list->SetPipelineState(this->plState.Get());

	D3D12_GPU_DESCRIPTOR_HANDLE cbuffHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(this->cbv_srvGPUHandle, this->nWVPIndex, this->nCBVHeapIncrementSize);
	D3D12_GPU_DESCRIPTOR_HANDLE texSamplerHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(this->samplerGPUHandle, this->nSamplerIndex, this->nCBVHeapIncrementSize);
	D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(this->cbv_srvGPUHandle, this->nTextureIndex, this->nCBVHeapIncrementSize);

	this->list->SetGraphicsRootDescriptorTable(0, cbuffHandle);
	this->list->SetGraphicsRootDescriptorTable(1, texSamplerHandle);
	this->list->SetGraphicsRootDescriptorTable(2, textureHandle);

	this->list->DrawInstanced(this->vertices.size(), 1, 0, 0);
}

void Mesh::UploadBuffers() {
	UINT nVerticesSize = this->vertices.size() * sizeof(Vertex);

	D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(nVerticesSize);

	ThrowIfFailed(this->dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(this->VBO.GetAddressOf())
	));

	this->VBO->SetName(L"Mesh VBO");

	PVOID ms;
	this->VBO->Map(NULL, nullptr, &ms);
	memcpy(ms, this->vertices.data(), nVerticesSize);
	this->VBO->Unmap(NULL, nullptr);

	this->VBV.BufferLocation = this->VBO->GetGPUVirtualAddress();
	this->VBV.SizeInBytes = nVerticesSize;
	this->VBV.StrideInBytes = sizeof(Vertex);

	return;
}

void Mesh::PrepareTextures() {
	this->nTextureIndex = this->core->GetNewHeapIndex(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE textureCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(this->cbv_srvCPUHandle, this->nTextureIndex, this->nCBVHeapIncrementSize);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = this->texture->GetDesc().Format;

	this->dev->CreateShaderResourceView(this->texture.Get(), &srvDesc, textureCPUHandle);

	D3D12_SAMPLER_DESC samplerDesc = { };
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NONE;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MaxAnisotropy = 1;

	this->nSamplerIndex = this->core->GetNewHeapIndex(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	D3D12_CPU_DESCRIPTOR_HANDLE texSamplerHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(this->samplerCPUHandle, this->nSamplerIndex, this->nSamplerIncrementSize);

	this->dev->CreateSampler(&samplerDesc, texSamplerHandle);
	
	return;
}


void Mesh::UpdateCBuffer() {
	/* Update our WVP */
	Camera* cam = this->sceneMgr->GetActualScene()->GetCamera();

	Vector3 localPos = this->transform->location;
	Vector3 localRot = this->transform->rotation;

	this->wvp.World = XMMatrixTranspose(XMMatrixIdentity() * XMMatrixTranslation(localPos.x, localPos.y, localPos.z));
	this->wvp.World *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(localRot.x)));
	this->wvp.World *= XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(localRot.y)));
	this->wvp.World *= XMMatrixTranspose(XMMatrixRotationZ(XMConvertToRadians(localRot.z)));
	cam->GetMatrices(this->wvp.View, this->wvp.Projection);

	/* Upload changes to GPU */
	UINT nAlignedCBuffSize = (sizeof(this->wvp) + 255) & ~255;

	PVOID ms;
	this->cbuffer->Map(NULL, nullptr, &ms);
	memcpy(ms, &this->wvp, nAlignedCBuffSize);
	this->cbuffer->Unmap(NULL, nullptr);

	ms = nullptr;

	return;
}

/*!
	This method will load a mesh from file
		Note: We only support FBX by the moment.
*/
void Mesh::LoadFromFile(std::string file) {
	if (this->bMeshLoaded) return;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file, NULL);

	aiMesh* mesh = scene->mMeshes[0]; // TODO: Add multi-mesh support.
	aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vec = mesh->mVertices[i];
		RGB pos = { vec.x, vec.y, vec.z };
		RGB nml = { 0.f, 0.f, 0.f };
		RG uv = { 0.f, 0.f };

		if (mesh->HasNormals()) {
			nml[0] = mesh->mNormals[i].x;
			nml[1] = mesh->mNormals[i].y;
			nml[2] = mesh->mNormals[i].z;
		}

		if (mesh->HasTextureCoords(0)) {
			uv[0] = mesh->mTextureCoords[0][i].x;
			uv[1] = mesh->mTextureCoords[0][i].y;
		}

		Vertex vertex = { { pos[0], pos[1], pos[2] }, { nml[0], nml[1], nml[2] }, { uv[0], uv[1] } };
		this->vertices.push_back(vertex);
	}

	/* Load our texture */
	aiString texPath;
	if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0 && mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
		const aiTexture* tex = scene->GetEmbeddedTexture(texPath.C_Str());

		ResourceUploadBatch batch(this->dev.Get());
		batch.Begin(D3D12_COMMAND_LIST_TYPE_DIRECT);
		ThrowIfFailed(CreateWICTextureFromMemory(this->dev.Get(), batch, (BYTE*)tex->pcData, tex->mWidth, this->texture.GetAddressOf()));
		batch.End(this->queue.Get());
	}

	this->bMeshLoaded = true;
}