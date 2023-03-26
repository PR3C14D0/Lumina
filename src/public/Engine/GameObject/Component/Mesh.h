#pragma once
#include <iostream>
#include <vector>
#include "Engine/GameObject/Component/Component.h"
#include "DirectXIncludes.h"
#include "Engine/Shader/Shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Engine/Vertex.h"

class Core;

class Mesh : public Component {
private:
	Core* core;

	ComPtr<ID3D12Device> dev;
	ComPtr<ID3D12GraphicsCommandList> list;

	std::vector<Vertex> vertices;

	Shader* shader;

	void Draw();
public:
	Mesh();

	void Start() override;
	void Update() override;

	bool bMeshLoaded;

	void LoadFromFile(std::string file);
};