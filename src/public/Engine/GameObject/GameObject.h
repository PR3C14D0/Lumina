#pragma once
#include <iostream>
#include "Math/Transform.h"
#include "DirectXIncludes.h"

class GameObject {
private:

public:
	std::string name;
	GameObject(std::string name);

	virtual void Start();
	virtual void Update();

	Transform transform;
};