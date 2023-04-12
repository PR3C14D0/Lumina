#pragma once
#include <iostream>
#include <map>
#include <wrl.h>
#include "DirectXIncludes.h"

using namespace Microsoft::WRL;

class ResourceManager {
private:
	std::map<std::string, ComPtr<ID3D12Resource>> resources;

	static ResourceManager* instance;
public:
	ResourceManager();
	static ResourceManager* GetInstance();

	bool ResourceExists(std::string name);
	void AddResource(ComPtr<ID3D12Resource>& resource, std::string name);
	void GetResource(std::string name, ComPtr<ID3D12Resource>& resource);
};