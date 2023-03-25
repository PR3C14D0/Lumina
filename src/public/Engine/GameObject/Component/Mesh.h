#pragma once
#include "Engine/GameObject/Component/Component.h"
#include "DirectXIncludes.h"

class Core;

class Mesh : public Component {
private:
	Core* core;
public:
	Mesh();

	void Start() override;
	void Update() override;
};