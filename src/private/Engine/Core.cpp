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
}

Core* Core::GetInstance() {
	if (Core::instance == nullptr)
		Core::instance = new Core();
	return Core::instance;
}