#include "Engine/ResourceManager.h"

ResourceManager* ResourceManager::instance;

ResourceManager::ResourceManager() {
	
}

bool ResourceManager::ResourceExists(std::string name) {
	if (this->resources.size() > 0) {
		if (this->resources.count(name) > 0)
			return true;
	}

	return false;
}

void ResourceManager::AddResource(ComPtr<ID3D12Resource>& resource, std::string name) {
	if (this->ResourceExists(name))
		return;

	this->resources[name] = resource;
	return;
}

void ResourceManager::GetResource(std::string name, ComPtr<ID3D12Resource>& resource) {
	if (this->ResourceExists(name))
		resource = this->resources[name];
	
	return;
}

ResourceManager* ResourceManager::GetInstance() {
	if (ResourceManager::instance == nullptr)
		ResourceManager::instance = new ResourceManager();
	return ResourceManager::instance;
}