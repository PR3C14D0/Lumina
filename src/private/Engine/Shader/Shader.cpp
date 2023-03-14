#include "Engine/Shader/Shader.h"
#include "Engine/Core.h"

Shader::Shader(const wchar_t* shaderFile, std::string vertexShaderMethod, std::string pixelShaderMethod) {
	this->core = Core::GetInstance();
	this->hwnd = this->core->GetHWND();

	WCHAR shaderPath[MAX_PATH];
	GetFullPathNameW(shaderFile, MAX_PATH, shaderPath, nullptr);

	ComPtr<ID3DBlob> VSErr, PSErr;
	ThrowIfFailed(D3DCompileFromFile(shaderPath, nullptr, nullptr, vertexShaderMethod.c_str(), "vs_5_1", NULL, NULL, this->VS.GetAddressOf(), VSErr.GetAddressOf()));
	ThrowIfFailed(D3DCompileFromFile(shaderPath, nullptr, nullptr, pixelShaderMethod.c_str(), "ps_5_1", NULL, NULL, this->PS.GetAddressOf(), PSErr.GetAddressOf()));

	if (VSErr) {
		MessageBox(this->hwnd, (const char*)VSErr->GetBufferPointer(), "Error", MB_OK);
		delete this;
		return;
	}

	if (PSErr) {
		MessageBox(this->hwnd, (const char*)PSErr->GetBufferPointer(), "Error", MB_OK);
		delete this;
		return;
	}
}

void Shader::GetBlob(ComPtr<ID3DBlob>& VS, ComPtr<ID3DBlob>& PS) {
	VS = this->VS;
	PS = this->PS;
	return;
}