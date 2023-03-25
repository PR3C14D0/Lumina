#pragma once
#include <iostream>
#include "Engine/GameObject/GameObject.h"
#include <directx/DirectXMath.h>

class Core;

using namespace DirectX;

class Camera : public GameObject {
public:
	Camera(std::string name);
	XMMATRIX View;
	XMMATRIX Projection;

	void Update() override;
};