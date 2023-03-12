#include "Engine/Core.h"

Core* Core::instance;

/*!
	Our core constructor.
*/
Core::Core() {
	this->hwnd = NULL;
	this->nNumBackBuffers = 2;

	this->width = 0;
	this->height = 0;
	
	/*!
		FBOs:
			- GBuffers:
				1 - Albedo
				2 - Normal
				3 - Position
			
			- ScreenQuad
	*/
	this->nNumGBuffers = static_cast<UINT>(GBUFFER_TYPE::GBUFFER_TYPE_LENGTH);
	this->nNumOtherFBOs = 1;
	this->nNumFBO = this->nNumGBuffers + this->nNumOtherFBOs;

	this->nNumRenderTargets = this->nNumFBO + this->nNumBackBuffers;

	this->nRTVHeapIncrementSize = 0;
	this->nDSVHeapIncrementSize = 0;
	this->nSamplerHeapIncrementSize = 0;
	this->nCBV_SRVHeapIncrementSize = 0;

	this->dsvActualIndex = 0;

	this->vsyncState = VSYNC::ENABLED;

	/* 
		We use the number of back buffers for using it as an offset. 
		We do this because if not, we will use our backbuffer indexes later and we don't want that. 

		We add 1 because it will be the next index.
	*/
	this->rtvActualIndex = this->nNumBackBuffers + 1;

	this->samplerActualIndex = 0;
	this->cbv_srvActualIndex = 0;

	this->nMultisampleCount = 8;
}

void Core::SetHWND(HWND& hwnd) {
	this->hwnd = hwnd;
	return;
}

/*!
	This method is our singleton initiazation method.
		Note: This method must be called once.
*/
void Core::Init() {
	if (!this->hwnd) {
		MessageBox(NULL, "No window found", "Error", MB_OK);
		return;
	}

	RECT rect;
	GetWindowRect(this->hwnd, &rect);

	this->width = rect.right - rect.left;
	this->height = rect.bottom - rect.top;

	UINT nFactoryCreateFlags = 0;

#ifndef NDEBUG
	nFactoryCreateFlags |= DXGI_CREATE_FACTORY_DEBUG;
	{
		ComPtr<ID3D12Debug1> dbg;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(dbg.GetAddressOf())));
		dbg->EnableDebugLayer();
		dbg->SetEnableGPUBasedValidation(TRUE);
	}
#endif // NDEBUG

	/* Creation of our device and factory */
	ThrowIfFailed(CreateDXGIFactory2(nFactoryCreateFlags, IID_PPV_ARGS(this->factory.GetAddressOf())));
	this->GetMostCapableAdapter(this->factory, this->adapter);

	D3D_FEATURE_LEVEL featureLevel = this->GetMaxFeatureLevel(this->adapter);

	ThrowIfFailed(D3D12CreateDevice(this->adapter.Get(), featureLevel, IID_PPV_ARGS(this->dev.GetAddressOf())));
	ThrowIfFailed(this->dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(this->alloc.GetAddressOf())));

	/* Creation of our command queue */
	D3D12_COMMAND_QUEUE_DESC queueDesc = { };
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(this->dev->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(this->queue.GetAddressOf())));

	/* Creation of our swap chain */
	{
		DXGI_SWAP_CHAIN_DESC1 scDesc = { };
		scDesc.BufferCount = this->nNumBackBuffers;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		scDesc.SampleDesc.Count = 1;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scDesc.Width = this->width;
		scDesc.Height = this->height;

		ComPtr<IDXGISwapChain1> sc;
		ThrowIfFailed(this->factory->CreateSwapChainForHwnd(
			this->queue.Get(),
			this->hwnd,
			&scDesc,
			nullptr,
			nullptr,
			sc.GetAddressOf()
		));

		sc.As(&this->sc);
	}

	this->InitDescriptorHeaps();

	/* Creation of our graphics command list*/
	ThrowIfFailed(this->dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, this->alloc.Get(), nullptr, IID_PPV_ARGS(this->list.GetAddressOf())));

	UINT nRTVIncrementSize = this->GetDescriptorIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUHandle = this->GetDescriptorCPUHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvCPUHandle, nRTVIncrementSize);
	for (int i = 0; i < this->nNumBackBuffers; i++) {
		ComPtr<ID3D12Resource> backBuffer;
		ThrowIfFailed(this->sc->GetBuffer(i, IID_PPV_ARGS(backBuffer.GetAddressOf())));

		this->backBuffers.push_back(backBuffer);

		this->dev->CreateRenderTargetView(this->backBuffers[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, nRTVIncrementSize);
	}

	/* Creation of our FBOs */
	D3D12_RENDER_TARGET_VIEW_DESC fboDesc = { };
	fboDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	fboDesc.Texture2D.MipSlice = 1;
	fboDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
	
	D3D12_RESOURCE_DESC fboResDesc = { };
	fboResDesc.SampleDesc.Count = this->nMultisampleCount;
	fboResDesc.MipLevels = 1;
	fboResDesc.DepthOrArraySize = 1;
	fboResDesc.Width = this->width;
	fboResDesc.Height = this->height;
	fboResDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	fboResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	fboResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	D3D12_HEAP_PROPERTIES fboProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	for (int i = 0; i < this->nNumGBuffers; i++ ) {
		ComPtr<ID3D12Resource> fbo;
		ThrowIfFailed(this->dev->CreateCommittedResource(
			&fboProps,
			D3D12_HEAP_FLAG_NONE,
			&fboResDesc,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			nullptr,
			IID_PPV_ARGS(fbo.GetAddressOf())
		));
		GBUFFER_TYPE type = static_cast<GBUFFER_TYPE>(i);

		this->GBuffers[type] = fbo;

		UINT index = this->GetNewHeapIndex(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		this->gbufferIndices[type] = index;
		CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffHandle(rtvCPUHandle, index, nRTVHeapIncrementSize);

		this->dev->CreateRenderTargetView(this->GBuffers[type].Get(), &fboDesc, GBuffHandle);
	}

	this->screenQuad = new ScreenQuad();
}

/*
	This method is for getting the specified G-Buffer;
		For more info about deferred rendering: https://en.wikipedia.org/wiki/Deferred_shading
*/
void Core::GetGBuffer(GBUFFER_TYPE type, ComPtr<ID3D12Resource>& outRes) {
	outRes = this->GBuffers[type];
	return;
}

/*!
	This method returns our core window.
*/
HWND Core::GetHWND() {
	return this->hwnd;
}

/*!
	This method gets a new descriptor heap index for specified descriptor heap type.
*/
UINT Core::GetNewHeapIndex(D3D12_DESCRIPTOR_HEAP_TYPE type) {
	UINT returnedIndex = 0;

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV) {
		returnedIndex = this->rtvActualIndex;
		this->rtvActualIndex++;
	}

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) {
		returnedIndex = this->cbv_srvActualIndex;
		this->cbv_srvActualIndex++;
	}

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) {
		returnedIndex = this->samplerActualIndex;
		this->samplerActualIndex++;
	}

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
		returnedIndex = this->dsvActualIndex;
		this->dsvActualIndex++;
	}

	return returnedIndex;
}

/*!
	This method will get the CPU handle of the specified descriptor heap type.
*/
D3D12_CPU_DESCRIPTOR_HANDLE Core::GetDescriptorCPUHandle(D3D12_DESCRIPTOR_HEAP_TYPE type) {
	D3D12_CPU_DESCRIPTOR_HANDLE handle = { };

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		handle = this->rtvHeap->GetCPUDescriptorHandleForHeapStart();

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
		handle = this->samplerHeap->GetCPUDescriptorHandleForHeapStart();

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		handle = this->cbv_srvHeap->GetCPUDescriptorHandleForHeapStart();

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
		handle = this->dsvHeap->GetCPUDescriptorHandleForHeapStart();

	return handle;
}

/*!
	This method will get the increment size for the specified descriptor heap type.
*/
UINT Core::GetDescriptorIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) {
	UINT size = 0;

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		size = this->nRTVHeapIncrementSize;

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
		size = this->nSamplerHeapIncrementSize;

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		size = this->nCBV_SRVHeapIncrementSize;

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
		size = this->nDSVHeapIncrementSize;

	return size;
}

/*!
	This method will get the GPU handle of the specified descriptor heap type.
*/
D3D12_GPU_DESCRIPTOR_HANDLE Core::GetDescriptorGPUHandle(D3D12_DESCRIPTOR_HEAP_TYPE type) {
	D3D12_GPU_DESCRIPTOR_HANDLE handle = { };

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		handle = this->rtvHeap->GetGPUDescriptorHandleForHeapStart();

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
		handle = this->samplerHeap->GetGPUDescriptorHandleForHeapStart();

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		handle = this->cbv_srvHeap->GetGPUDescriptorHandleForHeapStart();

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
		handle = this->dsvHeap->GetGPUDescriptorHandleForHeapStart();

	return handle;
}

/*!
	This method initializes our descriptor heaps
*/
void Core::InitDescriptorHeaps() {
	/* Render target view descriptor heap */
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = { };
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NumDescriptors = this->nNumRenderTargets;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	ThrowIfFailed(this->dev->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(this->rtvHeap.GetAddressOf())));

	/* Constant buffer view & shader resource view descriptor heap */
	D3D12_DESCRIPTOR_HEAP_DESC cbv_srvHeapDesc = { };
	cbv_srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbv_srvHeapDesc.NumDescriptors = D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_1;
	cbv_srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	
	ThrowIfFailed(this->dev->CreateDescriptorHeap(&cbv_srvHeapDesc, IID_PPV_ARGS(this->cbv_srvHeap.GetAddressOf())));

	/* Depth Stencil view descriptor heap */
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = { };
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ThrowIfFailed(this->dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(this->dsvHeap.GetAddressOf())));

	/* Sampler descriptor heap */
	D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = { };
	samplerHeapDesc.NumDescriptors = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE;
	samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;

	ThrowIfFailed(this->dev->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(this->samplerHeap.GetAddressOf())));

	/* Set increment sizes */
	this->nRTVHeapIncrementSize = this->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	this->nCBV_SRVHeapIncrementSize = this->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	this->nDSVHeapIncrementSize = this->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	this->nSamplerHeapIncrementSize = this->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	return;
}

/*!
	Our main core loop
		Note: This method will be called once per frame.
*/
void Core::MainLoop() {
	this->screenQuad->Render();
	ThrowIfFailed(sc->Present(1, 0));
}

/*!
	Gets the sample count specified before at the Core constructor.
*/
UINT Core::GetMultiSampleCount() {
	return this->nMultisampleCount;
}

/*!
	Gets our device and command list.
*/
void Core::GetDeviceAndCommandList(ComPtr<ID3D12Device>& dev, ComPtr<ID3D12GraphicsCommandList>& list) {
	dev = this->dev;
	list = this->list;
	return;
}

/*!
	This method gets our window size.
*/
void Core::GetWindowSize(int& width, int& height) {
	width = this->width;
	height = this->height;
}

/*!
	This method will get our most capable adapter.
	The adapter must be capable with the minimum feature level specified.
	If not, will show an error MessageBox
*/
void Core::GetMostCapableAdapter(ComPtr<IDXGIFactory2>& factory, ComPtr<IDXGIAdapter>& adapter) {
	if (adapter) return;

	D3D_FEATURE_LEVEL minimumFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	std::vector<ComPtr<IDXGIAdapter>> adapters;
	{
		ComPtr<IDXGIAdapter> tempAdapter;
		for (int i = 0; factory->EnumAdapters(i, tempAdapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; i++)
			adapters.push_back(tempAdapter);
	}

	for (ComPtr<IDXGIAdapter> tempAdapter : adapters) {
		if (SUCCEEDED(D3D12CreateDevice(tempAdapter.Get(), minimumFeatureLevel, __uuidof(ID3D12Device), nullptr))) {
			adapter = tempAdapter;
			break;
		}
	}

	if (!adapter)
		MessageBox(this->hwnd, "Your adapter must be at leas D3D_FEATURE_LEVEL_11_0", "Error", MB_OK);

	adapters.clear();

	return;
}

/*!
	This method will get the max feature level for the specified adapter.
		Note: The adapter must be capable with the minimum feature level for working.
*/
D3D_FEATURE_LEVEL Core::GetMaxFeatureLevel(ComPtr<IDXGIAdapter>& adapter) {
	D3D_FEATURE_LEVEL minimumFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_2
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelsData = { };
	featureLevelsData.pFeatureLevelsRequested = featureLevels;
	featureLevelsData.NumFeatureLevels = _countof(featureLevels);

	{
		ComPtr<ID3D12Device> dev;
		ThrowIfFailed(D3D12CreateDevice(adapter.Get(), minimumFeatureLevel, IID_PPV_ARGS(dev.GetAddressOf())));
		ThrowIfFailed(dev->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevelsData, sizeof(featureLevelsData)));
	}

	return featureLevelsData.MaxSupportedFeatureLevel;
}

Core* Core::GetInstance() {
	if (Core::instance == nullptr)
		Core::instance = new Core();
	return Core::instance;
}