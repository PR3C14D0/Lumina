#pragma once
#include <iostream>
#include <vector>
#include "Math/Transform.h"
#include "DirectXIncludes.h"
#include "Engine/GameObject/Component/Component.h"
#include "Engine/GameObject/Component/Mesh.h"

class GameObject {
private:
	std::vector<Component*> components;

public:
	std::string name;
	GameObject(std::string name);

	virtual void Start();
	virtual void Update();

	Transform transform;

	template<typename T>
	T* GetComponent();

	template<typename T>
	void AddComponent(Component* component);
};