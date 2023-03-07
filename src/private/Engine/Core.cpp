#include "Engine/Core.h"

Core* Core::instance;

Core::Core() {
	this->hwnd = NULL;
	this->nNumBackBuffers = 2;
	
	/*
		FBOs:
			- Albedo
			- Normal
			- Position
			- ScreenQuad
	*/
	this->nNumFBO = 4;

	this->nNumRenderTargets = this->nNumFBO + this->nNumBackBuffers;
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
}

/*!
	This method initializes our descriptor heaps
*/
void Core::InitDescriptorHeaps() {
	/* Render target view descriptor heap */
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = { };
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NumDescriptors = this->nNumBackBuffers;
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

	return;
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