#pragma once
#include <iostream>
#include <Windows.h>
#include "DirectXIncludes.h"
#include <d3dcompiler.h>
#include <wrl.h>
#include "Engine/Util.h"

using namespace Microsoft::WRL;

class Core;

class Shader {
private:
	Core* core;
	HWND hwnd;
	
	ComPtr<ID3DBlob> VS, PS;
public:
	Shader(const wchar_t* shaderFile, std::string vertexShaderMethod, std::string pixelShaderMethod);
	void GetBlob(ComPtr<ID3DBlob>& VS, ComPtr<ID3DBlob>& PS);
};