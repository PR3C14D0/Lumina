#include "Engine/Scene/SceneManager.h"
#include "Engine/Core.h"

SceneManager::SceneManager() {
	this->core = Core::GetInstance();
	this->hwnd = this->core->GetHWND();
	this->time = Time::GetInstance();
}

void SceneManager::Start() {
	this->sampleObj = new GameObject("Test");
	Scene* sampleScene = new Scene("asd");
	EditorCamera* sampleCamera = new EditorCamera("Camera");
	sampleScene->gameObjects["Editor Camera"] = sampleCamera;
	sampleScene->actualCamera = sampleCamera;
	Mesh* mesh = new Mesh(&sampleObj->transform);
	sampleScene->gameObjects["Test"] = this->sampleObj;
	this->actualScene = sampleScene;

	mesh->LoadFromFile("f16.fbx");
	sampleObj->AddComponent<Mesh>(mesh);
	mesh->Start();
	this->AddScene(sampleScene);
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
	this->sampleObj->transform.rotate(0.f, 5.f * time->deltaTime, 0.f);
	
	if (actualScene)
		this->actualScene->Update();
	else {
		MessageBox(this->hwnd, "SceneManager: No Scene specified", "Error", MB_OK);
		return;
	}
}

Scene* SceneManager::GetActualScene() {
	return this->actualScene;
}