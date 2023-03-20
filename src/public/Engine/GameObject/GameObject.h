#pragma once
#include <iostream>
#include "Math/Transform.h"

class GameObject {
public:
	std::string name;
	GameObject(std::string name);

	virtual void Start();
	virtual void Update();

	Transform transform;
};