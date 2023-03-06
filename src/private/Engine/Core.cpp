#include "Engine/Core.h"

Core* Core::instance;

Core::Core() {
	this->hwnd = NULL;
}

void Core::SetHWND(HWND& hwnd) {
	this->hwnd = hwnd;
	return;
}

void Core::Init() {
	if (!this->hwnd) {
		MessageBox(NULL, "No window found", "Error", MB_OK);
		return;
	}

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

	ThrowIfFailed(CreateDXGIFactory2(nFactoryCreateFlags, IID_PPV_ARGS(this->factory.GetAddressOf())));
	this->GetMostCapableAdapter(this->factory, this->adapter);

	D3D_FEATURE_LEVEL featureLevel = this->GetMaxFeatureLevel(this->adapter);

	ThrowIfFailed(D3D12CreateDevice(this->adapter.Get(), featureLevel, IID_PPV_ARGS(this->dev.GetAddressOf())));
}

/*
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

/*
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