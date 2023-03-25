#include "Engine/Scene/SceneManager.h"
#include "Engine/Core.h"

SceneManager::SceneManager() {
	this->core = Core::GetInstance();
	this->hwnd = this->core->GetHWND();
}

void SceneManager::SetActualScene(std::string name) {
	if (this->SceneExists(name)) {
		Scene* scene = this->scenes[name];
		this->actualScene = scene;
	}
	else
		MessageBox(this->hwnd, "Scene not found", "Error", MB_OK);
}

bool SceneManager::SceneExists(std::string name) {
	bool bRet = false;

	if (this->scenes.count(name) > 0)
		bRet = true;

	return bRet;
}

void SceneManager::AddScene(Scene* scene) {
	if (this->SceneExists(scene->name)) {
		MessageBox(this->hwnd, "A scene with that name already exists", "Error", MB_OK);
		return;
	}

	this->scenes[scene->name] = scene;
	return;
}

void SceneManager::Update() {
	if (actualScene)
		this->actualScene->Update();
	else {
		MessageBox(this->hwnd, "SceneManager: No Scene specified", "Error", MB_OK);
		return;
	}
}