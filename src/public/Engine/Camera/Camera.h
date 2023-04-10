#pragma once
#include <iostream>
#include "Engine/GameObject/GameObject.h"

class Core;

using namespace DirectX;

class Camera : public GameObject {
public:
	Camera(std::string name);
	XMMATRIX View;
	XMMATRIX Projection;

	void Update() override;

	void GetMatrices(XMMATRIX& View, XMMATRIX& Projection);
};