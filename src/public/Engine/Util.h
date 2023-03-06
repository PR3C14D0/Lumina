#pragma once
#include <iostream>
#include <Windows.h>

inline void ThrowIfFailed(HRESULT hr) {
	if (FAILED(hr))
		throw std::exception();
	return;
}