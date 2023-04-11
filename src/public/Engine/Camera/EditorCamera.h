#pragma once
#include "Engine/Camera/Camera.h"

class EditorCamera : public Camera {
public:
	EditorCamera(std::string name);

	void Start() override;
	void Update() override;
};